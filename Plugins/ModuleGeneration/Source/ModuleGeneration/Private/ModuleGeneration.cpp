// Copyright Dominik Peacock. All rights reserved.

#include "ModuleGeneration.h"

#include "ModuleGenerationCommands.h"
#include "NewModule/NewModuleUtils.h"

#include "Framework/Commands/UICommandList.h"
#include "Modules/ModuleManager.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "FModuleGenerationModule"

void FModuleGenerationModule::StartupModule()
{
	FModuleGenerationCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);
	PluginCommands->MapAction(
		FModuleGenerationCommands::Get().NewModule,
		FExecuteAction::CreateLambda([](){ UE::ModuleGeneration::CreateAndShowNewModuleWindow(); }),
		FCanExecuteAction());
	
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateLambda(
		[this]()
		{
			UToolMenu* FileMenu = UToolMenus::Get()->ExtendMenu("MainFrame.MainMenu.Tools");
			FToolMenuSection& Section = FileMenu->FindOrAddSection("Programming");
			Section.AddMenuEntryWithCommandList(FModuleGenerationCommands::Get().NewModule, PluginCommands);
		}
	));
}

void FModuleGenerationModule::ShutdownModule()
{}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FModuleGenerationModule, ModuleGeneration);