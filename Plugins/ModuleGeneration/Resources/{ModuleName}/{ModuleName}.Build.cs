// {Copyright}

using System.IO;
using UnrealBuildTool;

public class {ModuleName} : ModuleRules
{
	public {ModuleName}(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory, "Public") });
		PrivateIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory, "Private") });

		PublicDependencyModuleNames.AddRange( new string[] { "Core" });
		PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine" });

		DynamicallyLoadedModuleNames.AddRange( new string[] {  });
	}
}
