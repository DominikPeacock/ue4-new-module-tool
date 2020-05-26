﻿// Copyright (c) Dominik Peacock 2020

#include "ModuleGenerationCommands.h"

#define LOCTEXT_NAMESPACE "FModuleGenerationModule"

void FModuleGenerationCommands::RegisterCommands()
{
    UI_COMMAND(NewModule, "New C++ module...", "Creates a new game module in this project", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE