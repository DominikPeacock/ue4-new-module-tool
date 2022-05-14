// Copyright Dominik Peacock. All rights reserved.

#include "ModuleTemplateFileUtils.h"

#include "ModuleDescriptor.h"

#include "GeneralProjectSettings.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/FileHelper.h"

namespace UE::ModuleGeneration
{
	static void EnqueueSubdirectories(IFileManager& FileManager, const FString& ModuleTemplateDirectory, const FString& NextRelativeDirectory, TQueue<FString>& RelativeDirectoryQueue);
	static TArray<FString> FindFilesInDirectory(IFileManager& FileManager, const FString& ModuleTemplateDirectory, const FString& NextRelativeDirectory);

	FOperationResult InstantiateModuleTemplate(const FString& OutputDirectory, const FModuleDescriptor& NewModule)
	{
		// Setup string replacements for files and folders
		FStringFormatNamedArguments WildcardsToReplace;
		WildcardsToReplace.Add("ModuleName", FStringFormatArg(NewModule.Name.ToString()));
		WildcardsToReplace.Add("Copyright", FStringFormatArg(GetDefault<UGeneralProjectSettings>()->CopyrightNotice));

		// Get path to Resources/ModuleTemplate folder
		const FString& BasePluginDirectory = IPluginManager::Get().FindPlugin("ModuleGeneration")->GetBaseDir();
		const FString ModuleTemplateDirectory =
			FPaths::Combine(BasePluginDirectory, FString("Resources"));


		// Recursively search go through each sub-directory and copy over files...
		// ... replacing {ModuleName} and {Copyright} in the files
		TQueue<FString> RelativeDirectoryQueue;
		RelativeDirectoryQueue.Enqueue(FString("{ModuleName}"));
		IFileManager& FileManager = IFileManager::Get();

		while (!RelativeDirectoryQueue.IsEmpty())
		{
			const FString NextRelativeDirectory = [&RelativeDirectoryQueue]()
			{
				FString result;
				RelativeDirectoryQueue.Dequeue(result);
				return result;
			}();
			EnqueueSubdirectories(FileManager, ModuleTemplateDirectory, NextRelativeDirectory, RelativeDirectoryQueue);

			const TArray<FString> FilesInDirectory =
				FindFilesInDirectory(FileManager, ModuleTemplateDirectory, NextRelativeDirectory);

			const FString NewDirectoryName = FPaths::Combine(OutputDirectory, NextRelativeDirectory);
			const FString FormattedNewDirectoryName = FString::Format(*NewDirectoryName, WildcardsToReplace);
			if (!FileManager.DirectoryExists(*FormattedNewDirectoryName))
			{
				FileManager.MakeDirectory(*FormattedNewDirectoryName);
			}

			for (const auto FileToCopy : FilesInDirectory)
			{
				const FString FullFilePath = FPaths::Combine(ModuleTemplateDirectory, NextRelativeDirectory, FileToCopy);

				FString FileContents;
				TSharedPtr<FArchive> FileReadStream(FileManager.CreateFileReader(*FullFilePath));
				if (FileReadStream == nullptr)
				{
					return FOperationResult::MakeFailure(FString::Printf(TEXT("Failed to create file stream to template file '%s'"), *FullFilePath));
				}

				const bool bCouldReadFile = FFileHelper::LoadFileToString(FileContents, *FileReadStream.Get());
				if (!bCouldReadFile)
				{
					return FOperationResult::MakeFailure(FString::Printf(TEXT("Failed to read template file '%s'"), *FullFilePath));
				}

				const FString NewFileName = FString::Format(*FileToCopy, WildcardsToReplace);
				const FString NewFileFullPath = FPaths::Combine(FormattedNewDirectoryName, NewFileName);
				const FString NewFileContents = FString::Format(*FileContents, WildcardsToReplace);
				const bool bCouldWriteFile = FFileHelper::SaveStringToFile(NewFileContents, *NewFileFullPath);
				if (!bCouldWriteFile)
				{
					return FOperationResult::MakeFailure(FString::Printf(TEXT("Failed to write new to file '%s'"), *NewFileContents));
				}
			}
		}

		return FOperationResult::MakeSuccess();
	}
	
	static void EnqueueSubdirectories(IFileManager& FileManager, const FString& ModuleTemplateDirectory, const FString& NextRelativeDirectory, TQueue<FString>& RelativeDirectoryQueue)
	{
		const TArray<FString> Subdirectories = [&ModuleTemplateDirectory, &NextRelativeDirectory, &FileManager]()
		{
			TArray<FString> result;
			const FString FolderWildcard("*");
			const FString DirectoryWithWildcard = FPaths::Combine(ModuleTemplateDirectory, NextRelativeDirectory, FolderWildcard);
			FileManager.FindFiles(result, *DirectoryWithWildcard, false, true);
			return result;
		}();
		for (const auto Subdirectory : Subdirectories)
		{
			const FString RelativeDirectory = FPaths::Combine(NextRelativeDirectory, Subdirectory);
			RelativeDirectoryQueue.Enqueue(RelativeDirectory);
		}
	}
	
	static TArray<FString> FindFilesInDirectory(IFileManager& FileManager, const FString& ModuleTemplateDirectory, const FString& NextRelativeDirectory)
	{
		TArray<FString> result;
		const FString FileWildcard("*.*");
		const FString DirectoryWithWildcard = FPaths::Combine(ModuleTemplateDirectory, NextRelativeDirectory, FileWildcard);
		FileManager.FindFiles(result, *DirectoryWithWildcard, true, false);
		return result;
	}
}
