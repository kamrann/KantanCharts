// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

using UnrealBuildTool;

public class KantanChartsUMG : ModuleRules
{
	public KantanChartsUMG(ReadOnlyTargetRules Target): base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

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
