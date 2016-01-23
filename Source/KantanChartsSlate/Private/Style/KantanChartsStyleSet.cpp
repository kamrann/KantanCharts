
#include "KantanChartsSlate.h"
#include "KantanChartsStyleSet.h"
#include "SlateGameResources.h"


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

TSharedRef< FSlateStyleSet > FKantanChartsStyleSet::Create()
{
	FString BasePath = TEXT("/KantanCharts/Style/ChartStyles");
	FString ScopePath = BasePath;
	TSharedRef<FSlateStyleSet> Style = FSlateGameResources::New(
		FKantanChartsStyleSet::GetStyleSetName(),
		ScopePath,
		BasePath
		);

	// @NOTE: These currently unused I believe, using assets instead.
	// @TODO: Perhaps this should live in a separate, editor-only style set?
	// regardless not working, displaying blank icon, currently working around by using asset for image and slate brush
	Style->Set("Widget.KantanBarChart", new IMAGE_BRUSH(TEXT("icon_umg_kantanbarchart_16x"), Icon16x16));
	Style->Set("Widget.KantanPieChart", new IMAGE_BRUSH(TEXT("icon_umg_kantanpiechart_16x"), Icon16x16));
	Style->Set("Widget.KantanCartesianChart", new IMAGE_BRUSH(TEXT("icon_umg_kantancartesianchart_16x"), Icon16x16));
	Style->Set("Widget.KantanCartesianChart", new IMAGE_BRUSH(TEXT("icon_umg_kantantimeseriesplot_16x"), Icon16x16));

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
	/*/ @TODO: Temp workaround for monolithic build initialization order issue.
	// UObject CDOs are getting constructed before a module that their own module has a 
	// dependency on is initialized... 
	if (!StyleInstance.IsValid())
	{
		Initialize();
	}
	/*/

	return *StyleInstance;
}


