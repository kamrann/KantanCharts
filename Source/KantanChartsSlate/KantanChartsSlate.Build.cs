// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

using UnrealBuildTool;

public class KantanChartsSlate : ModuleRules
{
	public KantanChartsSlate(TargetInfo Target)
	{
        PrivateIncludePaths.Add("KantanChartsSlate/Private");

		PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "RHI",
				"RenderCore",
                "Projects",     // For IPluginManager
                "KantanChartsDatasource"
            }
        );

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Slate",
				"SlateCore"
			}
		);
	}
}
