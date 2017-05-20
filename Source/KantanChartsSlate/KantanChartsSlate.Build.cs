// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class KantanChartsSlate : ModuleRules
{
	public KantanChartsSlate(ReadOnlyTargetRules Target): base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));

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
