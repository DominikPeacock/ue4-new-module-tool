// Copyright Dominik Peacock. All rights reserved.

#include "NewModule/NewModuleUtils.h"
#include "NewModule/SNewModuleDialog.h"
#include "Logging.h"

#include "ModuleDescriptor.h"

#include "Dom/JsonValue.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopedSlowTask.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWindow.h"
#include "GameProjectGenerationModule.h"
#include "ModuleTemplateFileUtils.h"
#include "Interfaces/IMainFrameModule.h"

#define LOCTEXT_NAMESPACE "FModuleGenerationModule"

namespace UE::ModuleGeneration
{
	TSharedRef<SWindow> CreateAndShowNewModuleWindow()
	{
		const FVector2D WindowSize(940, 380); // 480
		const FText WindowTitle = LOCTEXT("NewModule_Title", "New C++ Module");

		const TSharedRef<SWindow> AddCodeWindow =
			SNew(SWindow)
			.Title(WindowTitle)
			.ClientSize(WindowSize)
			.SizingRule(ESizingRule::FixedSize)
			.SupportsMinimize(false)
			.SupportsMaximize(false);

		const TSharedRef<SNewModuleDialog> NewModuleDialog =
			SNew(SNewModuleDialog)
			.ParentWindow(AddCodeWindow)
			.OnClickFinished(SNewModuleDialog::FOnRequestNewModule::CreateLambda([](const FString& Directory, const FModuleDescriptor& ModuleDescriptor)
			{
				return CreateNewModule(Directory, ModuleDescriptor);
			}));
		AddCodeWindow->SetContent(NewModuleDialog);

		const IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
		if (const TSharedPtr<SWindow> ParentWindow = MainFrameModule.GetParentWindow())
		{
			FSlateApplication::Get().AddWindowAsNativeChild(AddCodeWindow, ParentWindow.ToSharedRef());
		}
		else
		{
			FSlateApplication::Get().AddWindow(AddCodeWindow);
		}

		return AddCodeWindow;
	}

	// Declare helper functions for NewModuleModel::CreateNewModule 
	namespace
	{
		namespace EModuleCreationLocation
		{
			enum Type
			{
				/**
				 * Module was created nowhere; an error occured.
				 */
				Nowhere,
				/**
				 * In .uproject
				 */
				Project,
				/**
				 * In .uplugin
				 */
				Plugin
			};
		}
	}

	static TOperationResult<EModuleCreationLocation::Type> CreateNewModuleInternal(const FString& OutputDirectory, const FModuleDescriptor& NewModule);
	
	FOperationResult CreateNewModule(const FString& OutputDirectory, const FModuleDescriptor& NewModule)
	{
		const TOperationResult<EModuleCreationLocation::Type> CreationLocation =
			CreateNewModuleInternal(OutputDirectory, NewModule);
		if (CreationLocation.IsFailure())
		{
			return FOperationResult::MakeFailure(CreationLocation);
		}
		
		if (CreationLocation.OperationResult.GetValue() == EModuleCreationLocation::Project)
		{
			const FText DoneMessageUnformatted = LOCTEXT("NewModule_Done_ModuleMessage", "Sucessfully created new module.\n\nYou need to update your project's .Target.cs files by adding:\nExtraModuleNames.Add(\"{0}\")");
			const FText DoneMessage = FText::Format(FTextFormat(DoneMessageUnformatted), FText::FromString(NewModule.Name.ToString()));
			const FText DoneTitle = LOCTEXT("NewModule_Done_Title", "New C++ Module");
			FMessageDialog::Open(EAppMsgType::Ok, DoneMessage, &DoneTitle);
			return FOperationResult::MakeSuccess();
		}
		if (CreationLocation.OperationResult.GetValue() == EModuleCreationLocation::Plugin)
		{
			const FText DoneMessageUnformatted = LOCTEXT("NewModule_Done_PluginMessage", "Sucessfully created new module.");
			const FText DoneMessage = FText::Format(FTextFormat(DoneMessageUnformatted), FText::FromString(NewModule.Name.ToString()));
			const FText DoneTitle = LOCTEXT("NewModule_Done_Title", "New C++ Module");
			FMessageDialog::Open(EAppMsgType::Ok, DoneMessage, &DoneTitle);
			return FOperationResult::MakeSuccess();
		}

		checkNoEntry();
		return FOperationResult::MakeFailure(TEXT("Enum entry missing"));
	}

	TOperationResult<EModuleCreationLocation::Type> CreateNewModuleInternal(const FString& OutputDirectory, const FModuleDescriptor& NewModule)
	{
		FScopedSlowTask ProgressBar(3, LOCTEXT("NewModule_ProgressBar_DefaultTitle", "Creating new module files..."));
		ProgressBar.MakeDialog();
		UE_LOG(LogModuleGeneration, Log, TEXT("Creating new module '%s'..."), *NewModule.Name.ToString());

		ProgressBar.EnterProgressFrame(1, LOCTEXT("NewModule_ProgressBar_CopyingFiles", "Copying files..."));
		const FOperationResult CopyModuleTemplateOp = InstantiateModuleTemplate(OutputDirectory, NewModule);
		if (!CopyModuleTemplateOp)
		{
			return TOperationResult<EModuleCreationLocation::Type>::MakeFailure(CopyModuleTemplateOp);
		}

		EModuleCreationLocation::Type AddedLocation;
		if (OutputDirectory.Contains("/Plugins/"))
		{
			AddedLocation = EModuleCreationLocation::Plugin;
			ProgressBar.EnterProgressFrame(1, LOCTEXT("NewModule_ProgressBar_WrittingPluginFiles", "Writting plugin files..."));
			const FOperationResult OperationResult = AddNewModuleToUPluginJsonFile(OutputDirectory, NewModule);
			if (!OperationResult)
			{
				return TOperationResult<EModuleCreationLocation::Type>::MakeFailure(OperationResult);
			}
		}
		else
		{
			AddedLocation = EModuleCreationLocation::Project;
			ProgressBar.EnterProgressFrame(1, LOCTEXT("NewModule_ProgressBar_WrittingProjectFiles", "Writting project files..."));
			const FOperationResult OperationResult = AddNewModuleToUProjectJsonFile(NewModule);
			if (!OperationResult)
			{
				return TOperationResult<EModuleCreationLocation::Type>::MakeFailure(OperationResult);
			}
		}

		ProgressBar.EnterProgressFrame(1, LOCTEXT("NewModule_ProgressBar_GeneratingVisualStudioSolutionFiles", "Generating visual studio solution..."));
		GenerateVisualStudioSolution();

		return TOperationResult<EModuleCreationLocation::Type>::MakeSuccess(AddedLocation);
	}

	FOperationResult AddNewModuleToUProjectJsonFile(const FModuleDescriptor& NewModule)
	{
		IFileManager& FileManager = IFileManager::Get();
		
		const FString ProjectDirectory = UKismetSystemLibrary::GetProjectDirectory();
		const FString ProjectSearchRegex = FPaths::Combine(ProjectDirectory, FString("*.uproject"));
		TArray<FString> UProjectFileNames;
		FileManager.FindFiles(UProjectFileNames, *ProjectSearchRegex, true, false);

		if(UProjectFileNames.Num() == 0)
		{
			return FOperationResult::MakeFailure(FString::Printf(TEXT("Failed to locate .uproject file for current project. Searched path: '%s'"), *ProjectSearchRegex));
		}
		if(UProjectFileNames.Num() > 1)
		{
			UE_LOG(LogModuleGeneration, Warning, TEXT("Found multiple .uproject files. Picking '%s'..."), *UProjectFileNames[0]);
		}

		const FString PathToProjectFile = FPaths::Combine(ProjectDirectory, UProjectFileNames[0]);
		return AddNewModuleToFile(PathToProjectFile, NewModule);
	}
	
	FOperationResult AddNewModuleToUPluginJsonFile(const FString& OutputDirectory, const FModuleDescriptor& NewModule)
	{
		IFileManager& FileManager = IFileManager::Get();

		FString Separator = "/Plugins/";
		FString LeftString, RightString = "";
		OutputDirectory.Split(Separator, &LeftString, &RightString, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		FString PluginName, RightPluginName = "";
		const FString NewSeparator = "/";
		RightString.Split(NewSeparator, &PluginName, &RightPluginName);

		const FString ProjectDirectory = UKismetSystemLibrary::GetProjectDirectory();
		const FString PluginFolderDirectory = FPaths::Combine(ProjectDirectory, FString("Plugins"), PluginName);
		const FString ProjectSearchRegex = FPaths::Combine(PluginFolderDirectory, FString("*.uplugin"));
		TArray<FString> UPluginFileNames;
		FileManager.FindFiles(UPluginFileNames, *ProjectSearchRegex, true, false);

		if (UPluginFileNames.Num() == 0)
		{
			return FOperationResult::MakeFailure(FString::Printf(TEXT("Failed to locate .uplugin file for current project. Searched path: '%s'"), *ProjectSearchRegex));
		}
		if (UPluginFileNames.Num() > 1)
		{
			UE_LOG(LogModuleGeneration, Warning, TEXT("Found multiple .uplugin files. Picking '%s'..."), *UPluginFileNames[0]);
		}

		const FString PathToProjectFile = FPaths::Combine(PluginFolderDirectory, UPluginFileNames[0]);
		return AddNewModuleToFile(PathToProjectFile, NewModule);
	}
	
	FOperationResult AddNewModuleToFile(const FString& FullFilePath, const FModuleDescriptor& NewModule)
	{
		IFileManager& FileManager = IFileManager::Get();
		
		FString FileContents;
		TSharedPtr<FArchive> FileReadStream(FileManager.CreateFileReader(*FullFilePath));
		if (FileReadStream == nullptr)
		{
			return FOperationResult::MakeFailure(FString::Printf(TEXT("Failed to create file stream to config file '%s'"), *FullFilePath));
		}
		const bool bCouldReadFile = FFileHelper::LoadFileToString(FileContents, *FileReadStream.Get());
		if (!bCouldReadFile)
		{
			return FOperationResult::MakeFailure(FString::Printf(TEXT("Failed to read config file '%s'"), *FullFilePath));
		}

		TSharedPtr<FJsonObject> ProjectFileAsJson;
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(*FileContents);
		if (!FJsonSerializer::Deserialize(JsonReader, ProjectFileAsJson))
		{
			return FOperationResult::MakeFailure(FString::Printf(TEXT("Failed to parse JSON from config file '%s'"), *FullFilePath));
		}

		TArray<TSharedPtr<FJsonValue>> Modules = ProjectFileAsJson->GetArrayField("Modules");
		const bool bModuleAlreadyInList = [&Modules, &NewModule]()
		{
			for (auto e : Modules)
			{
				const TSharedPtr<FJsonObject>* PtrToJsonObject;
				e->TryGetObject(PtrToJsonObject);
				if (PtrToJsonObject == nullptr)
				{
					continue;
				}

				TSharedPtr<FJsonObject> AsJsonObject = *PtrToJsonObject;
				FString ModuleName;
				if (AsJsonObject->TryGetStringField("Name", ModuleName)
					&& ModuleName.Equals(NewModule.Name.ToString()))
				{
					return true;
				}
			}
			return false;
		}();
		if (bModuleAlreadyInList)
		{
			return FOperationResult::MakeFailure(FString::Printf(TEXT("The config file at '%s' already contained an entry '%s'"), *FullFilePath, *NewModule.Name.ToString()));
		}

		TSharedPtr<FJsonObject> ModuleAsJson(new FJsonObject);
		ModuleAsJson->SetStringField("Name", NewModule.Name.ToString());
		ModuleAsJson->SetStringField("Type", EHostType::ToString(NewModule.Type));
		ModuleAsJson->SetStringField("LoadingPhase", ELoadingPhase::ToString(NewModule.LoadingPhase));
		Modules.Add(TSharedPtr<FJsonValueObject>(new FJsonValueObject(ModuleAsJson)));
		ProjectFileAsJson->SetArrayField("Modules", Modules);

		FString OutputString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(ProjectFileAsJson.ToSharedRef(), Writer);
		FFileHelper::SaveStringToFile(OutputString, *FullFilePath);

		return FOperationResult::MakeSuccess();
	}

	FOperationResult GenerateVisualStudioSolution()
	{
		FText FailReason, FailLog;
		if (!FGameProjectGenerationModule::Get().UpdateCodeProject(FailReason, FailLog))
		{
			return FOperationResult::MakeFailure(FString::Printf(TEXT("Failed to update visual studio solution. You need to regenerate the solution manually.\n\nError reason: '%s'"), *FailReason.ToString()));
		}
		return FOperationResult::MakeSuccess();
	}
}


#undef LOCTEXT_NAMESPACE