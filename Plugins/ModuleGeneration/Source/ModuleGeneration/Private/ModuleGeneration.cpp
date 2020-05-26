// Copyright (c) Dominik Peacock 2020

#include "ModuleGeneration.h"

#include "Logging.h"
#include "ModuleGenerationCommands.h"
#include "NewModule/NewModuleLogic.h"

#include "Modules/ModuleManager.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "FModuleGenerationModule"

void FModuleGenerationModule::StartupModule()
{
	FModuleGenerationCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);
	PluginCommands->MapAction(
		FModuleGenerationCommands::Get().NewModule,
		FExecuteAction::CreateStatic(&NewModuleController::CreateAndShowNewModuleWindow),
		FCanExecuteAction());
	
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateLambda(
		[this]()
		{
			UToolMenu* FileMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.File");
			FToolMenuSection& Section = FileMenu->FindOrAddSection("FileProject");
			Section.AddMenuEntryWithCommandList(FModuleGenerationCommands::Get().NewModule, PluginCommands);
		}
	));
}

void FModuleGenerationModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FModuleGenerationModule, ModuleGeneration);