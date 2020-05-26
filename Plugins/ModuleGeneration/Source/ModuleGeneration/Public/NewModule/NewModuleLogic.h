// Copyright (c) Dominik Peacock 2020

#pragma once

#include "NewModule/NewModuleEvents.h"

struct FModuleDescriptor;

namespace NewModuleController
{
	void CreateAndShowNewModuleWindow();
}

namespace NewModuleModel
{
	void CreateNewModule(const FString& OutputDirectory, const FModuleDescriptor& NewModuleName, const FOnCreateNewModuleError& ErrorHandler);
}