// {Copyright}

#include "{ModuleName}.h"
#include "Logging.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "F{ModuleName}"

void F{ModuleName}::StartupModule()
{
}

void F{ModuleName}::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(F{ModuleName}, {ModuleName});