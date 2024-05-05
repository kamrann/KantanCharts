// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class KantanChartsEditor : ModuleRules
{
	public KantanChartsEditor(ReadOnlyTargetRules Target): base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",        // Seemingly needed for SNumericEntryBox
            "Slate",
            "SlateCore",
            "UnrealEd",
            "PropertyEditor",
            "Projects",
            "XmlParser",

            "KantanChartsDatasource",
            "KantanChartsSlate",
            "KantanChartsUMG",
        });

        DynamicallyLoadedModuleNames.AddRange(new string[] {
			// ... add any modules that your module loads dynamically here ...
            "MainFrame",
            "ContentBrowser",
            "AssetTools",
            "AssetRegistry",
        });

        PrivateIncludePathModuleNames.AddRange(new string[] {
            "MainFrame",
            "ContentBrowser",
            "AssetTools",
            "AssetRegistry",
        });
	}
}
