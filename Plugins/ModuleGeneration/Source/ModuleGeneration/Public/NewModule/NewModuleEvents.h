// Copyright (c) Dominik Peacock 2020

#pragma once

#include "CoreMinimal.h"

struct FModuleDescriptor;
class FString;

DECLARE_DELEGATE_OneParam(FOnCreateNewModuleError, const FString& /* ErrorMessage */)
DECLARE_DELEGATE_ThreeParams(FOnRequestNewModule, const FString& /*OutputDirectory*/, const FModuleDescriptor& /*ClassPath*/, const FOnCreateNewModuleError& /* ErrorCallback */ );



