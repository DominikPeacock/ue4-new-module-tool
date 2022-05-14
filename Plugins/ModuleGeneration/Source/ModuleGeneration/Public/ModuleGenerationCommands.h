// Copyright Dominik Peacock. All rights reserved.

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

    TSharedPtr<FUICommandInfo> NewModule;
};

