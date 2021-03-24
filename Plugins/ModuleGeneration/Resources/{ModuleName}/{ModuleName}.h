// {Copyright}

#pragma once

#include "CoreMinimal.h"

#include "Modules/ModuleManager.h"
#include "Logging/LogMacros.h"

DECLARE_LOG_CATEGORY_EXTERN({ModuleName}, Log, All);

class F{ModuleName} : public IModuleInterface
{
public:

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

};
