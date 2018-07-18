
#include "Style/KantanChartsStyleSet.h"
#include "Style/KantanBarChartStyle.h"
#include "Style/KantanCartesianChartStyle.h"

#include "Slate/SlateGameResources.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"
#include "Framework/Application/SlateApplication.h"


TSharedPtr< FSlateStyleSet > FKantanChartsStyleSet::StyleInstance = nullptr;


void FKantanChartsStyleSet::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FKantanChartsStyleSet::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FKantanChartsStyleSet::GetStyleSetName()
{
	static FName StyleSetName(TEXT("KantanChartsStyleSet"));
	return StyleSetName;
}


#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

static TSharedPtr< IPlugin > GetPluginFromModuleName(const FName& ModuleName)
{
	auto PluginStatuses = IPluginManager::Get().GetDiscoveredPlugins();
	for(auto const& Plugin : PluginStatuses)
	{
		if(Plugin->GetDescriptor().Modules.ContainsByPredicate([&ModuleName](const FModuleDescriptor& Mod)
		{
			return Mod.Name == ModuleName;
		}))
		{
			return Plugin;
		}
	}

	return nullptr;
}

TSharedRef< FSlateStyleSet > FKantanChartsStyleSet::Create()
{
	auto Plugin = GetPluginFromModuleName(TEXT("KantanChartsSlate"));
	check(Plugin.IsValid());
	FString BasePath = Plugin->GetContentDir() / TEXT("Style");
	FString ScopePath = BasePath;
	TSharedRef<FSlateStyleSet> Style = FSlateGameResources::New(
		FKantanChartsStyleSet::GetStyleSetName(),
		ScopePath,
		BasePath
		);

	// Store some default chart styles

	// For default game style, use the built in defaults, which are white lines overlaid on transparent background.
	FKantanBarChartStyle GameBarChartStyle = FKantanBarChartStyle::GetDefault();
	Style->Set("BarChart.DefaultGame", GameBarChartStyle);
	
	FKantanCartesianChartStyle GameCartesianChartStyle = FKantanCartesianChartStyle::GetDefault();
	Style->Set("CartesianChart.DefaultGame", GameCartesianChartStyle);

	// For default editor style, we use a white background with black lines and text.
	FSlateColorBrush EdBgBrush = FSlateColorBrush{ FLinearColor::White };
	FLinearColor EdLineCol = FLinearColor::Black;
	FLinearColor EdTextCol = FLinearColor::Black;

	FKantanBarChartStyle EditorBarChartStyle = FKantanBarChartStyle::GetDefault();
	EditorBarChartStyle.Background = EdBgBrush;
	EditorBarChartStyle.ChartLineColor = EdLineCol;
	EditorBarChartStyle.FontColor = EdTextCol;
	EditorBarChartStyle.BarOpacity = 1.0f;
	EditorBarChartStyle.BarOutlineOpacity = 1.0f;
	Style->Set("BarChart.DefaultEditor", EditorBarChartStyle);
	
	FKantanCartesianChartStyle EditorCartesianChartStyle = FKantanCartesianChartStyle::GetDefault();
	EditorCartesianChartStyle.Background = EdBgBrush;
	EditorCartesianChartStyle.ChartLineColor = EdLineCol;
	EditorCartesianChartStyle.FontColor = EdTextCol;
	EditorCartesianChartStyle.DataOpacity = 1.0f;
	Style->Set("CartesianChart.DefaultEditor", EditorCartesianChartStyle);

	// @TODO: Perhaps this should live in a separate, editor-only style set?
	Style->Set("ClassIcon.BarChart", new IMAGE_BRUSH(TEXT("editor/icon_umg_kantanbarchart_16x"), Icon16x16));
	Style->Set("ClassIcon.SimpleBarChart", new IMAGE_BRUSH(TEXT("editor/icon_umg_kantanbarchart_16x"), Icon16x16));
	//Style->Set("ClassIcon.PieChart", new IMAGE_BRUSH(TEXT("editor/icon_umg_kantanpiechart_16x"), Icon16x16));
	//Style->Set("ClassIcon.SimplePieChart", new IMAGE_BRUSH(TEXT("editor/icon_umg_kantanpiechart_16x"), Icon16x16));
	Style->Set("ClassIcon.CartesianPlot", new IMAGE_BRUSH(TEXT("editor/icon_umg_kantancartesianchart_16x"), Icon16x16));
	Style->Set("ClassIcon.SimpleCartesianPlot", new IMAGE_BRUSH(TEXT("editor/icon_umg_kantancartesianchart_16x"), Icon16x16));
	Style->Set("ClassIcon.TimeSeriesPlot", new IMAGE_BRUSH(TEXT("editor/icon_umg_kantantimeseriesplot_16x"), Icon16x16));
	Style->Set("ClassIcon.SimpleTimeSeriesPlot", new IMAGE_BRUSH(TEXT("editor/icon_umg_kantantimeseriesplot_16x"), Icon16x16));
	Style->Set("ClassIcon.KantanChartLegend", new IMAGE_BRUSH(TEXT("editor/icon_umg_chartlegend_16x"), Icon16x16));

	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT


void FKantanChartsStyleSet::ReloadTextures()
{
	FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
}

const ISlateStyle& FKantanChartsStyleSet::Get()
{
	// @TODO: Temp workaround for monolithic build initialization order issue.
	// UObject CDOs are getting constructed before a module that their own module has a 
	// dependency on is initialized... 
	if (!StyleInstance.IsValid())
	{
		Initialize();
	}
	//

	return *StyleInstance;
}


