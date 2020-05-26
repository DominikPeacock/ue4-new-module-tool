// {Copyright}

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class F{ModuleName} : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	void StartupModule() override;
	void ShutdownModule() override;
	
};
