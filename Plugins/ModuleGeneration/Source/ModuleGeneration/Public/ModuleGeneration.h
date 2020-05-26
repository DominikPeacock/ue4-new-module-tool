// Copyright (c) Dominik Peacock 2020

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FModuleGenerationModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	void StartupModule() override;
	void ShutdownModule() override;

private:
	
	TSharedPtr<class FUICommandList> PluginCommands;
	
};
