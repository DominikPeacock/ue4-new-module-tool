// Copyright (c) Dominik Peacock 2020

using UnrealBuildTool;

public class ModuleGeneration : ModuleRules
{
	public ModuleGeneration(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePaths.AddRange(
            new string[]
            {
                "ModuleGeneration/Private"
            }
        );

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
				"EditorStyle",
                "Slate"
			}
			);
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"AppFramework",
				"Core",
                "CoreUObject",
				"DesktopPlatform",
                "Engine",
				"EngineSettings",
				"GameProjectGeneration",
                "InputCore",
				"Json",
                "Projects",
				"UnrealEd",
                "Slate",
                "SlateCore",
                "ToolMenus"
			}
			);
	}
}
