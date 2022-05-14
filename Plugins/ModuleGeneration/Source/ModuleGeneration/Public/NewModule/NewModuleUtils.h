// Copyright Dominik Peacock. All rights reserved.

#pragma once

#include "OperationResult.h"
#include "NewModule/NewModuleEvents.h"

struct FModuleDescriptor;

namespace UE::ModuleGeneration
{
	/**
	 * Creates a new window containing the new module wizard UI. The new window is parented to the editor's main window.
	 */
	TSharedRef<SWindow> CreateAndShowNewModuleWindow();

	/**
	 * 
	 */
	FOperationResult CreateNewModule(const FString& OutputDirectory, const FModuleDescriptor& NewModuleName);

	FOperationResult AddNewModuleToUProjectJsonFile(const FModuleDescriptor& NewModule);
	FOperationResult AddNewModuleToUPluginJsonFile(const FString& OutputDirectory, const FModuleDescriptor& NewModule);
	FOperationResult AddNewModuleToFile(const FString& FullFilePath, const FModuleDescriptor& NewModule);
	
	FOperationResult GenerateVisualStudioSolution();
}