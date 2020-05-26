// Copyright (c) Dominik Peacock 2020

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "EditorStyleSet.h"

class FModuleGenerationCommands : public TCommands<FModuleGenerationCommands>
{
public:

    FModuleGenerationCommands()
        : TCommands<FModuleGenerationCommands>(TEXT("ModuleGeneration"), NSLOCTEXT("Contexts", "ModuleGenerationCommands", "Module Generation Commands"), NAME_None, FEditorStyle::GetStyleSetName())
    {}
    void RegisterCommands() override;
    
public:

    TSharedPtr<FUICommandInfo> NewModule;
    
};

