// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

using UnrealBuildTool;

public class KantanChartsEditor : ModuleRules
{
	public KantanChartsEditor(ReadOnlyTargetRules Target): base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "UnrealEd",
                "PropertyEditor",
                "EditorStyle",
                "InputCore",        // Seemingly needed for SNumericEntryBox
                "ContentBrowser",
                "KantanChartsDatasource",
                "KantanChartsSlate",
				"KantanChartsUMG"
            }
        );

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
	}
}
