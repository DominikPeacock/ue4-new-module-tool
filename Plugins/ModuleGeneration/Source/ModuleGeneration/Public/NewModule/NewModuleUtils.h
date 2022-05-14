// Copyright Dominik Peacock. All rights reserved.

#pragma once

#include "NewModule/NewModuleEvents.h"

struct FModuleDescriptor;

namespace UE::ModuleGeneration
{
	TSharedRef<SWindow> CreateAndShowNewModuleWindow();
	
	void CreateNewModule(const FString& OutputDirectory, const FModuleDescriptor& NewModuleName, const FOnCreateNewModuleError& ErrorHandler);
}