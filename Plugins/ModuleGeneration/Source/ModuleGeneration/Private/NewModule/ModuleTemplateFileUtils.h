// Copyright Dominik Peacock. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "NewModule/OperationResult.h"

struct FModuleDescriptor;

namespace UE::ModuleGeneration
{
	/**
	 * Copies the template modules files to a specific location.
	 */
	FOperationResult InstantiateModuleTemplate(const FString& OutputDirectory, const FModuleDescriptor& NewModule);
}
