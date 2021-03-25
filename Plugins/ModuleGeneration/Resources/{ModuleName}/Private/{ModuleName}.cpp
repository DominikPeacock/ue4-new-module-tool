// {Copyright}

#include "{ModuleName}.h"
#include "Logging.h"

#define LOCTEXT_NAMESPACE "F{ModuleName}"

void F{ModuleName}::StartupModule()
{
    UE_LOG({ModuleName}, Warning, TEXT("{ModuleName} has started!"));
}

void F{ModuleName}::ShutdownModule()
{
	UE_LOG({ModuleName}, Warning, TEXT("{ModuleName} has started!"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(F{ModuleName}, {ModuleName});
