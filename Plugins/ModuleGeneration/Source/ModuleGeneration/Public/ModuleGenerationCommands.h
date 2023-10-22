// Copyright Dominik Peacock. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Styling/AppStyle.h"

class FModuleGenerationCommands : public TCommands<FModuleGenerationCommands>
{
public:

    FModuleGenerationCommands()
        : TCommands<FModuleGenerationCommands>(TEXT("ModuleGeneration"), NSLOCTEXT("Contexts", "ModuleGenerationCommands", "Module Generation Commands"), NAME_None, FAppStyle::GetAppStyleSetName())
    {}
    void RegisterCommands() override;

    TSharedPtr<FUICommandInfo> NewModule;
};

