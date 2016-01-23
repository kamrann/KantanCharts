// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

using UnrealBuildTool;

public class KantanChartsUMG : ModuleRules
{
	public KantanChartsUMG(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "UMG",
				"KantanChartsSlate",
                "KantanChartsDatasource"
            }
        );

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
	}
}
