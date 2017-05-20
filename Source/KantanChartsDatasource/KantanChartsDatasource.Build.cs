// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

using UnrealBuildTool;

public class KantanChartsDatasource : ModuleRules
{
	public KantanChartsDatasource(ReadOnlyTargetRules Target): base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject"
            }
        );
	}
}

