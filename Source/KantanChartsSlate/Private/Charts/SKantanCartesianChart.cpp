// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

#include "KantanChartsSlate.h"
#include "SKantanCartesianChart.h"
#include "KantanCartesianDatasourceInterface.h"
#include "SimpleRenderTarget.h"
#include "FloatRoundingLevel.h"
#include "SlateRotatedRect.h"
#include "AxisUtility.h"


auto const AxisMarkerLength = 5.0f;
auto const AxisMarkerLabelGap = 2.0f;	// Size of gap to leave between the end of a marker line and the start of the label text

// @NOTE: Needed to avoid clipping lines along extreme edges of the chart.
FMargin const ChartClipRectExtension = FMargin{ 0.0f, 0.0f, 1.0f, 1.0f };


class FDataSeriesElement : public ICustomSlateElement
{
public:
	FDataSeriesElement();
	~FDataSeriesElement();

	struct FRenderData
	{
		FTexture* TextureResource;
		FVector2D UV_0;
		FVector2D UV_1;
		FLinearColor Color;
		FVector2D PointSize;
		TArray< FVector2D > Points;
	};

	/**
	* Sets up the canvas for rendering
	*
	* @param	InCanvasRect			Size of the canvas tile
	* @param	InClippingRect			How to clip the canvas tile
	* @param	InExpressionPreview		Render proxy for the Material preview
	* @param	bInIsRealtime			Whether preview is using realtime values
	*
	* @return	Whether there is anything to render
	*/
	bool BeginRenderingCanvas(const FIntRect& InCanvasRect, const FIntRect& InClippingRect, /*FSlateRenderTransform const& RenderTransform,*/ UTexture2D* InTexture, FVector2D InUV_0, FVector2D InUV_1, FLinearColor InColor, FVector2D InPointSize, TArray< FVector2D >&& InPoints);

private:
	/**
	* ICustomSlateElement interface
	*/
	virtual void DrawRenderThread(FRHICommandListImmediate& RHICmdList, const void* InWindowBackBuffer) override;

private:
	/** Render target that the canvas renders to */
	TUniquePtr< class FSimpleRenderTarget > RenderTarget;

	FRenderData RenderData;
};


SKantanCartesianChart::~SKantanCartesianChart()
{
	// Pass ownership of the series elements to the render thread so that they're deleted when the
	// render thread is done with them
	TArray< FSeriesElemPtr > ElemList;
	for (auto const& Elem : SeriesElements)
	{
		ElemList.Add(Elem.Value);
	}

	ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER
		(
		SafeDeleteSeriesElements,
		TArray< FSeriesElemPtr >, Elements, ElemList,
		{
			Elements.Empty();
		}
	);
}

void SKantanCartesianChart::Construct(const FArguments& InArgs)
{
	SKantanChart::Construct(
		SKantanChart::FArguments()
		.UpdateTickRate(InArgs._UpdateTickRate)
		);

	SetStyle(InArgs._Style);
	SetDatasource(InArgs._Datasource);
	SetUseAutoPerSeriesStyles(true);
	SetPlotScale(InArgs._PlotScale);
	SetDataPointSize(InArgs._DataPointSize);
	SetAxisTitlePadding(FMargin(0.0f, 4.0f));
	SetAntialiasDataLines(true);
}

void SKantanCartesianChart::SetStyle(const FKantanCartesianChartStyle* InStyle)
{
	Style = InStyle;
}

bool SKantanCartesianChart::SetDatasource(UObject* InDatasource)
{
	if (IsNullOrValidDatasource(InDatasource) == false)
	{
		return false;
	}

	if (Datasource != InDatasource)
	{
		Datasource = InDatasource;

		if (Datasource)
		{
			// Immediately update the data snapshot
			DataSnapshot.UpdateFromDatasource(Datasource);
		}
		else
		{
			DataSnapshot.Clear();
		}

		UpdateDrawingElementsFromDatasource();
		UpdateSeriesConfigFromDatasource();
	}

	return true;
}

void SKantanCartesianChart::SetUseAutoPerSeriesStyles(bool bEnable)
{
	bAutoPerSeriesStyles = bEnable;
}

void SKantanCartesianChart::SetSeriesStylesList(TArray< FKantanSeriesStyle > const& Styles)
{
	SeriesStyles = Styles;
}

void SKantanCartesianChart::SetManualSeriesStyleMappings(TMap< FName, FName > const& Mappings)
{
	for (auto const& Mp : Mappings)
	{
		auto const& Id = Mp.Key;
		auto& Cfg = SeriesConfig.FindOrAdd(Id);
		Cfg.SeriesStyleId = Mp.Value;
	}
}

void SKantanCartesianChart::SetPlotScale(FKantanCartesianPlotScale const& Scaling)
{
	PlotScale = Scaling;
}

void SKantanCartesianChart::SetDataPointSize(EKantanDataPointSize::Type InSize)
{
	DataPointSize = InSize;
}

void SKantanCartesianChart::SetAxisTitlePadding(FMargin const& InPadding)
{
	AxisTitlePadding = InPadding;
}

void SKantanCartesianChart::SetXAxisConfig(FCartesianAxisConfig const& InConfig)
{
	XAxisCfg = InConfig;
}

void SKantanCartesianChart::SetYAxisConfig(FCartesianAxisConfig const& InConfig)
{
	YAxisCfg = InConfig;
}

void SKantanCartesianChart::SetAntialiasDataLines(bool bEnable)
{
	bAntialiasDataLines = bEnable;
}

void SKantanCartesianChart::EnableSeries(FName Id, bool bEnable)
{
	auto& Cfg = SeriesConfig.FindOrAdd(Id);
	Cfg.bEnabled = bEnable;
}

void SKantanCartesianChart::ConfigureSeries(FName Id, bool bDrawPoints, bool bDrawLines)
{
	auto& Cfg = SeriesConfig.FindOrAdd(Id);
	Cfg.bDrawPoints = bDrawPoints;
	Cfg.bDrawLines = bDrawLines;
}

void SKantanCartesianChart::SetSeriesStyle(FName Id, FName StyleId)
{
	auto& Cfg = SeriesConfig.FindOrAdd(Id);
	Cfg.SeriesStyleId = StyleId;
}

void SKantanCartesianChart::ResetSeries(FName Id)
{
	if (Id == NAME_None)
	{
		SeriesConfig.Empty();
	}
	else
	{
		SeriesConfig.Remove(Id);
	}
}

bool SKantanCartesianChart::IsSeriesEnabled(FName Id) const
{
	auto Cfg = SeriesConfig.Find(Id);
	return Cfg && Cfg->bEnabled;
}

bool SKantanCartesianChart::IsSeriesShowingLines(FName Id) const
{
	auto Cfg = SeriesConfig.Find(Id);
	return Cfg && Cfg->bDrawLines;
}

bool SKantanCartesianChart::IsSeriesShowingPoints(FName Id) const
{
	auto Cfg = SeriesConfig.Find(Id);
	return Cfg && Cfg->bDrawPoints;
}


int32 SKantanCartesianChart::GetNumSeries() const
{
/*	return DatasourceInterface ?
		IKantanCartesianDatasourceInterface::Execute_GetNumSeries(DatasourceInterface) :
		0;
		*/
	return DataSnapshot.Elements.Num();
}

FName SKantanCartesianChart::GetSeriesId(int32 Index) const
{
/*	return DatasourceInterface ?
		IKantanCartesianDatasourceInterface::Execute_GetSeriesId(DatasourceInterface, Index) :
		NAME_None;
		*/
	return DataSnapshot.Elements[Index].Id;
}

FText SKantanCartesianChart::GetSeriesLabel(int32 Index) const
{
/*	return DatasourceInterface ?
		IKantanCartesianDatasourceInterface::Execute_GetSeriesName(DatasourceInterface, Index) :
		FText();
		*/
	return DataSnapshot.Elements[Index].Name;
}

TArray< FKantanCartesianDatapoint > SKantanCartesianChart::GetSeriesDatapoints(int32 Index) const
{
/*	return DatasourceInterface ?
		IKantanCartesianDatasourceInterface::Execute_GetSeriesDatapoints(DatasourceInterface, Index) :
		TArray < FKantanCartesianDatapoint > {};
		*/
	return DataSnapshot.Elements[Index].Points;
}

FKantanSeriesStyle const& SKantanCartesianChart::GetSeriesStyle(FName SeriesId) const
{
	// @TODO prob temp should be chart property
	static FKantanSeriesStyle DefaultStyle = FKantanSeriesStyle();

	if (SeriesId.IsNone() == false)
	{
		auto CfgEntry = SeriesConfig.Find(SeriesId);
		if (CfgEntry && CfgEntry->SeriesStyleId.IsNone() == false)
		{
			auto Style = FindSeriesStyle(CfgEntry->SeriesStyleId);
			if (Style)
			{
				return *Style;
			}
		}
	}

	return DefaultStyle;
}

bool SKantanCartesianChart::IsValidDatasource(UObject* Source)
{
	return Source != nullptr &&
		Source->GetClass()->ImplementsInterface(IKantanCartesianDatasourceInterface::UClassType::StaticClass());
}

bool SKantanCartesianChart::IsNullOrValidDatasource(UObject* Source)
{
	return Source == nullptr || IsValidDatasource(Source);
}

void SKantanCartesianChart::UpdateDrawingElementsFromDatasource()
{
	auto NumSeries = GetNumSeries();
	for (int32 Idx = 0; Idx < NumSeries; ++Idx)
	{
		auto SeriesId = GetSeriesId(Idx);

		if (SeriesId.IsNone() == false && SeriesElements.Contains(SeriesId) == false)
		{
			// Series not in map

			// Create new slate rendering element and add to map
			auto Element = MakeShareable< FDataSeriesElement >(new FDataSeriesElement());
			SeriesElements.Add(SeriesId, Element);
		}
	}

	// @TODO: Remove unused?
}

void SKantanCartesianChart::UpdateSeriesConfigFromDatasource()
{
	// Loop through all series in the datasource
	auto NumSeries = GetNumSeries();
	for (int32 Idx = 0; Idx < NumSeries; ++Idx)
	{
		auto SeriesId = GetSeriesId(Idx);
		if (SeriesId.IsNone() == false)
		{
			auto Cfg = SeriesConfig.Find(SeriesId);
			if (Cfg == nullptr)
			{
				// Not configured, set up default config
				Cfg = &SeriesConfig.Add(SeriesId, FSeriesConfig{});
			}
			
			if (bAutoPerSeriesStyles && Cfg->SeriesStyleId.IsNone())
			{
				Cfg->SeriesStyleId = GetNextSeriesStyle();
			}
		}
	}

	// @TODO: Better to remove configs for series no longer in the data source, or leave them in?
}

FName SKantanCartesianChart::GetNextSeriesStyle() const
{
	auto IsStyleUsed = [this](FName const& StyleId)
	{
		for (auto const& Cfg : SeriesConfig)
		{
			if (Cfg.Value.SeriesStyleId == StyleId)
			{
				return true;
			}
		}

		return false;
	};

	for (auto const& Style : SeriesStyles)
	{
		if (IsStyleUsed(Style.StyleId) == false)
		{
			// This style currently not being used
			return Style.StyleId;
		}
	}

	// Currently if no unused styles available, return None (which will lead to using default style)
	return NAME_None;
}

int32 SKantanCartesianChart::DrawChartArea(
	EChartContentArea::Type Area,
	const FPaintArgs& Args,
	const FGeometry& Geometry,
	const FGeometry& PlotSpaceGeometry,
	const FSlateRect& MyClippingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled
	) const
{
	// Used to track the layer ID we will return.
	int32 RetLayerId = LayerId;

	bool bEnabled = ShouldBeEnabled(bParentEnabled);
	const ESlateDrawEffect::Type DrawEffects = bEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

	// Pre-snap the clipping rect to try and reduce common jitter, since the padding is typically only a single pixel.
	FSlateRect SnappedClippingRect = FSlateRect(FMath::RoundToInt(MyClippingRect.Left), FMath::RoundToInt(MyClippingRect.Top), FMath::RoundToInt(MyClippingRect.Right), FMath::RoundToInt(MyClippingRect.Bottom));

/*	FGeometry AreaGeoms[ChartContentAreaCount];
	MakeAreaGeometries(ContentGeometry, AreaGeoms);
	auto const& PlotSpaceGeometry = AreaGeoms[EChartContentArea::Plot];
*/
	if (PlotSpaceGeometry.GetLocalSize().X == 0
		|| PlotSpaceGeometry.GetLocalSize().Y == 0)
	{
		// @TODO: Bit of a cheap way out, avoiding some division by zero issues
		return RetLayerId;
	}

	switch (Area)
	{
	case EChartContentArea::XAxisBottomTitle:
		if (XAxisCfg.LeftBottomAxis.bEnabled && XAxisCfg.LeftBottomAxis.bShowTitle)
		{
			DrawXAxisTitle(Geometry, SnappedClippingRect, OutDrawElements, RetLayerId, XAxisCfg.Title, XAxisCfg.Unit, DetermineAxisMarkerData(PlotSpaceGeometry, EAxis::X));
		}
		break;
	case EChartContentArea::XAxisTopTitle:
		if (XAxisCfg.RightTopAxis.bEnabled && XAxisCfg.RightTopAxis.bShowTitle)
		{
			DrawXAxisTitle(Geometry, SnappedClippingRect, OutDrawElements, RetLayerId, XAxisCfg.Title, XAxisCfg.Unit, DetermineAxisMarkerData(PlotSpaceGeometry, EAxis::X));
		}
		break;
	case EChartContentArea::YAxisLeftTitle:
		if (YAxisCfg.LeftBottomAxis.bEnabled && YAxisCfg.LeftBottomAxis.bShowTitle)
		{
			DrawYAxisTitle(Geometry, SnappedClippingRect, OutDrawElements, RetLayerId, YAxisCfg.Title, YAxisCfg.Unit, DetermineAxisMarkerData(PlotSpaceGeometry, EAxis::Y));
		}
		break;
	case EChartContentArea::YAxisRightTitle:
		if (YAxisCfg.RightTopAxis.bEnabled && YAxisCfg.RightTopAxis.bShowTitle)
		{
			DrawYAxisTitle(Geometry, SnappedClippingRect, OutDrawElements, RetLayerId, YAxisCfg.Title, YAxisCfg.Unit, DetermineAxisMarkerData(PlotSpaceGeometry, EAxis::Y));
		}
		break;

	case EChartContentArea::XAxisBottom:
		if (XAxisCfg.LeftBottomAxis.bEnabled)
		{
			DrawFixedAxis(
				Geometry,
				SnappedClippingRect,
				OutDrawElements,
				RetLayerId,
				EAxis::X,
				AxisUtil::FAxisTransform::FromTransform2D(CartesianToPlotTransform(PlotSpaceGeometry), 0 /* X axis */),
				EChartAxisPosition::LeftBottom,
				DetermineAxisMarkerData(PlotSpaceGeometry, EAxis::X),
				XAxisCfg.LeftBottomAxis.bShowMarkers,
				XAxisCfg.LeftBottomAxis.bShowLabels,
				AxisMarkerLength,
				AxisMarkerLabelGap
				);
		}
		break;
	case EChartContentArea::XAxisTop:
		if (XAxisCfg.RightTopAxis.bEnabled)
		{
			DrawFixedAxis(
				Geometry,
				SnappedClippingRect,
				OutDrawElements,
				RetLayerId,
				EAxis::X,
				AxisUtil::FAxisTransform::FromTransform2D(CartesianToPlotTransform(PlotSpaceGeometry), 0 /* X axis */),
				EChartAxisPosition::RightTop,
				DetermineAxisMarkerData(PlotSpaceGeometry, EAxis::X),
				XAxisCfg.RightTopAxis.bShowMarkers,
				XAxisCfg.RightTopAxis.bShowLabels,
				AxisMarkerLength,
				AxisMarkerLabelGap
				);
		}
		break;
	case EChartContentArea::YAxisLeft:
		if (YAxisCfg.LeftBottomAxis.bEnabled)
		{
			DrawFixedAxis(
				Geometry,
				SnappedClippingRect,
				OutDrawElements,
				RetLayerId,
				EAxis::Y,
				AxisUtil::FAxisTransform::FromTransform2D(CartesianToPlotTransform(PlotSpaceGeometry), 1 /* Y axis */),
				EChartAxisPosition::LeftBottom,
				DetermineAxisMarkerData(PlotSpaceGeometry, EAxis::Y),
				YAxisCfg.LeftBottomAxis.bShowMarkers,
				YAxisCfg.LeftBottomAxis.bShowLabels,
				AxisMarkerLength,
				AxisMarkerLabelGap
				);
		}
		break;
	case EChartContentArea::YAxisRight:
		if (YAxisCfg.RightTopAxis.bEnabled)
		{
			DrawFixedAxis(
				Geometry,
				SnappedClippingRect,
				OutDrawElements,
				RetLayerId,
				EAxis::Y,
				AxisUtil::FAxisTransform::FromTransform2D(CartesianToPlotTransform(PlotSpaceGeometry), 1 /* Y axis */),
				EChartAxisPosition::RightTop,
				DetermineAxisMarkerData(PlotSpaceGeometry, EAxis::Y),
				YAxisCfg.RightTopAxis.bShowMarkers,
				YAxisCfg.RightTopAxis.bShowLabels,
				AxisMarkerLength,
				AxisMarkerLabelGap
				);
		}
		break;

	case EChartContentArea::Plot:
	{
		// @TODO: Using deprecated GetClippingRect which ignores render transforms.
		// Not sure of better way to do this though.
		SnappedClippingRect = PlotSpaceGeometry.GetClippingRect().IntersectionWith(SnappedClippingRect);

		// Add 1 unit to right and bottom of clip rect for purposes of drawing axes
		auto AxisLayer = RetLayerId;
		FPlotMarkerData PlotMarkerData;
		PlotMarkerData.XAxis = DetermineAxisMarkerData(PlotSpaceGeometry, EAxis::X);
		PlotMarkerData.YAxis = DetermineAxisMarkerData(PlotSpaceGeometry, EAxis::Y);
		RetLayerId = DrawAxes(PlotSpaceGeometry, SnappedClippingRect.ExtendBy(FMargin(0, 0, 1, 1)), OutDrawElements, AxisLayer, AxisLayer + 2, PlotMarkerData);

		auto ChartStyle = GetChartStyle();
		auto NumSeries = GetNumSeries();

		for (int32 Idx = 0; Idx < NumSeries; ++Idx)
		{
			auto SeriesId = GetSeriesId(Idx);
			if (SeriesId.IsNone())
			{
				continue;
			}

			auto const& Config = SeriesConfig[SeriesId];
			if (Config.bEnabled == false)
			{
				continue;
			}

			// Don't render if no element is setup for this series
			if (SeriesElements.Contains(SeriesId) == false)
			{
				continue;
			}

			auto Points = GetSeriesDatapoints(Idx);
			auto const& Style = GetSeriesStyle(SeriesId);

			// @TODO: Sort out layers, maybe need to separate out DrawAxes into DrawAxisLines and DrawAxisLabels
			DrawSeries(PlotSpaceGeometry, SnappedClippingRect, OutDrawElements, AxisLayer + 1, SeriesId, Points, Style);
		}
	}
	break;
	}

	return RetLayerId;
}

#if 0
int32 SKantanCartesianChart::DrawChartArea(
	EChartContentArea::Type Area,
	const FPaintArgs& Args,
	const FGeometry& ContentGeometry,
	const FSlateRect& MyClippingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled
	) const
{
	// Used to track the layer ID we will return.
	int32 RetLayerId = LayerId;

	bool bEnabled = ShouldBeEnabled(bParentEnabled);
	const ESlateDrawEffect::Type DrawEffects = bEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

	// Pre-snap the clipping rect to try and reduce common jitter, since the padding is typically only a single pixel.
	FSlateRect SnappedClippingRect = FSlateRect(FMath::RoundToInt(MyClippingRect.Left), FMath::RoundToInt(MyClippingRect.Top), FMath::RoundToInt(MyClippingRect.Right), FMath::RoundToInt(MyClippingRect.Bottom));

	/*	FGeometry AreaGeoms[ChartContentAreaCount];
	MakeAreaGeometries(ContentGeometry, AreaGeoms);
	auto const& PlotSpaceGeometry = AreaGeoms[EChartContentArea::Plot];

	if (PlotSpaceGeometry.GetLocalSize().X == 0
	|| PlotSpaceGeometry.GetLocalSize().Y == 0)
	{
	// @TODO: Bit of a cheap way out, avoiding some division by zero issues
	return RetLayerId;
	}
	*/


	FPlotMarkerData MarkerData;
	MarkerData.XAxis = DetermineAxisMarkerData(PlotSpaceGeometry, EAxis::X);
	MarkerData.YAxis = DetermineAxisMarkerData(PlotSpaceGeometry, EAxis::Y);

	if (XAxisCfg.LeftBottomAxis.bEnabled)
	{
		if (XAxisCfg.LeftBottomAxis.bShowTitle)
		{
			DrawXAxisTitle(AreaGeoms[EChartContentArea::XAxisBottomTitle], SnappedClippingRect, OutDrawElements, RetLayerId, MarkerData);
		}

		DrawFixedAxis(
			AreaGeoms[EChartContentArea::XAxisBottom],
			SnappedClippingRect,
			OutDrawElements,
			RetLayerId,
			EAxis::X,
			AxisUtil::FAxisTransform::FromTransform2D(CartesianToPlotTransform(PlotSpaceGeometry), 0 /* X axis */),
			EChartAxisPosition::LeftBottom,
			MarkerData.XAxis,
			XAxisCfg.LeftBottomAxis.bShowMarkers,
			XAxisCfg.LeftBottomAxis.bShowLabels,
			AxisMarkerLength,
			AxisMarkerLabelGap
			);
	}

	if (XAxisCfg.RightTopAxis.bEnabled)
	{
		if (XAxisCfg.RightTopAxis.bShowTitle)
		{
			DrawXAxisTitle(AreaGeoms[EChartContentArea::XAxisTopTitle], SnappedClippingRect, OutDrawElements, RetLayerId, MarkerData);
		}

		DrawFixedAxis(
			AreaGeoms[EChartContentArea::XAxisTop],
			SnappedClippingRect,
			OutDrawElements,
			RetLayerId,
			EAxis::X,
			AxisUtil::FAxisTransform::FromTransform2D(CartesianToPlotTransform(PlotSpaceGeometry), 0 /* X axis */),
			EChartAxisPosition::RightTop,
			MarkerData.XAxis,
			XAxisCfg.RightTopAxis.bShowMarkers,
			XAxisCfg.RightTopAxis.bShowLabels,
			AxisMarkerLength,
			AxisMarkerLabelGap
			);
	}

	if (YAxisCfg.LeftBottomAxis.bEnabled)
	{
		if (YAxisCfg.LeftBottomAxis.bShowTitle)
		{
			DrawYAxisTitle(AreaGeoms[EChartContentArea::YAxisLeftTitle], SnappedClippingRect, OutDrawElements, RetLayerId, MarkerData);
		}

		DrawFixedAxis(
			AreaGeoms[EChartContentArea::YAxisLeft],
			SnappedClippingRect,
			OutDrawElements,
			RetLayerId,
			EAxis::Y,
			AxisUtil::FAxisTransform::FromTransform2D(CartesianToPlotTransform(PlotSpaceGeometry), 1 /* Y axis */),
			EChartAxisPosition::LeftBottom,
			MarkerData.YAxis,
			YAxisCfg.LeftBottomAxis.bShowMarkers,
			YAxisCfg.LeftBottomAxis.bShowLabels,
			AxisMarkerLength,
			AxisMarkerLabelGap
			);
	}

	if (YAxisCfg.RightTopAxis.bEnabled)
	{
		if (YAxisCfg.RightTopAxis.bShowTitle)
		{
			DrawYAxisTitle(AreaGeoms[EChartContentArea::YAxisRightTitle], SnappedClippingRect, OutDrawElements, RetLayerId, MarkerData);
		}

		DrawFixedAxis(
			AreaGeoms[EChartContentArea::YAxisRight],
			SnappedClippingRect,
			OutDrawElements,
			RetLayerId,
			EAxis::Y,
			AxisUtil::FAxisTransform::FromTransform2D(CartesianToPlotTransform(PlotSpaceGeometry), 1 /* Y axis */),
			EChartAxisPosition::RightTop,
			MarkerData.YAxis,
			YAxisCfg.RightTopAxis.bShowMarkers,
			YAxisCfg.RightTopAxis.bShowLabels,
			AxisMarkerLength,
			AxisMarkerLabelGap
			);
	}

	// @TODO: Using deprecated GetClippingRect which ignores render transforms.
	// Not sure of better way to do this though.
	SnappedClippingRect = PlotSpaceGeometry.GetClippingRect().IntersectionWith(SnappedClippingRect);

	// Add 1 unit to right and bottom of clip rect for purposes of drawing axes
	auto AxisLayer = RetLayerId;
	RetLayerId = DrawAxes(PlotSpaceGeometry, SnappedClippingRect.ExtendBy(FMargin(0, 0, 1, 1)), OutDrawElements, AxisLayer, AxisLayer + 2, MarkerData);

	auto ChartStyle = GetChartStyle();
	auto NumSeries = GetNumSeries();

	for (int32 Idx = 0; Idx < NumSeries; ++Idx)
	{
		auto SeriesId = GetSeriesId(Idx);
		if (SeriesId.IsNone())
		{
			continue;
		}

		auto const& Config = SeriesConfig[SeriesId];
		if (Config.bEnabled == false)
		{
			continue;
		}

		// Don't render if no element is setup for this series
		if (SeriesElements.Contains(SeriesId) == false)
		{
			continue;
		}

		auto Points = GetSeriesDatapoints(Idx);
		auto const& Style = GetSeriesStyle(SeriesId);

		// @TODO: Sort out layers, maybe need to separate out DrawAxes into DrawAxisLines and DrawAxisLabels
		DrawSeries(PlotSpaceGeometry, SnappedClippingRect, OutDrawElements, AxisLayer + 1, SeriesId, Points, Style);
	}

	return RetLayerId;
}
#endif

/**
 * Computes the desired size of this widget (SWidget)
 *
 * @return  The widget's desired size
 */
FVector2D SKantanCartesianChart::ComputeDesiredSize( float ) const
{
	return FVector2D(300, 300);
}

void SKantanCartesianChart::OnActiveTick(double InCurrentTime, float InDeltaTime)
{
	if (Datasource != nullptr)
	{
		DataSnapshot.UpdateFromDatasource(Datasource);

		UpdateDrawingElementsFromDatasource();
		UpdateSeriesConfigFromDatasource();
	}
}

void SKantanCartesianChart::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SKantanChart::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

/*
int32 SKantanCartesianChart::DrawAxisLabels(const FGeometry& ContentGeometry, const FSlateRect& ClipRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, FPlotMarkerData const& MarkerData) const
{
	auto FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	auto ChartStyle = GetChartStyle();
	auto LabelFont = GetLabelFont(ChartStyle, EKantanChartLabelClass::AxisTitle);

	if (XAxisLabel.IsEmptyOrWhitespace() == false)
	{
		auto XAxisLabelGeometry = MakeXAxisLabelGeometry(ContentGeometry);

		auto Label = XAxisLabel;
		if (true)	// @TODO: config
		{
			FText UnitStr = FText::GetEmpty();
			if (XAxisUnit.IsEmptyOrWhitespace() == false)
			{
				UnitStr = FText::Format(FText::FromString(TEXT("{0}")),
					XAxisUnit
					);
			}
			FText ExponentStr = FText::GetEmpty();
			if (MarkerData.XAxis.DisplayPower != 0)
			{
				ExponentStr = FText::Format(FText::FromString(TEXT("x10^{0}")),
					FText::AsNumber(MarkerData.XAxis.DisplayPower)
					);
			}
			FText UnitSpace = FText::GetEmpty();
			if (!UnitStr.IsEmpty() && !ExponentStr.IsEmpty())
			{
				UnitSpace = FText::FromString(TEXT(" "));
			}
			if (!UnitStr.IsEmpty() || !ExponentStr.IsEmpty())
			{
				Label = FText::Format(FText::FromString(TEXT("{0} ({1}{2}{3})")),
					Label,
					ExponentStr,
					UnitSpace,
					UnitStr
					);
			}
		}

		auto Extents = FontMeasureService->Measure(Label, LabelFont);
		auto AvailableSize = XAxisLabelGeometry.GetLocalSize();
		FSlateDrawElement::MakeText(
			OutDrawElements,
			LayerId,
			XAxisLabelGeometry.ToPaintGeometry(Extents, FSlateLayoutTransform(FVector2D((AvailableSize.X - Extents.X) * 0.5f, 0.0f))),
			Label,
			LabelFont,
			ClipRect,
			ESlateDrawEffect::None,
			ChartStyle->FontColor);
	}

	if (YAxisLabel.IsEmptyOrWhitespace() == false)
	{
		auto YAxisLabelGeometry = MakeYAxisLabelGeometry(ContentGeometry);

		auto Label = YAxisLabel;
		if (true)
		{
			FText UnitStr = FText::GetEmpty();
			if (YAxisUnit.IsEmptyOrWhitespace() == false)
			{
				UnitStr = FText::Format(FText::FromString(TEXT(" {0}")),
					YAxisUnit
					);
			}
			FText ExponentStr = FText::GetEmpty();
			if (MarkerData.YAxis.DisplayPower != 0)
			{
				ExponentStr = FText::Format(FText::FromString(TEXT("x10^{0}")),
					FText::AsNumber(MarkerData.YAxis.DisplayPower)
					);
			}
			FText UnitSpace = FText::GetEmpty();
			if (!UnitStr.IsEmpty() && !ExponentStr.IsEmpty())
			{
				UnitSpace = FText::FromString(TEXT(" "));
			}
			if (!UnitStr.IsEmpty() || !ExponentStr.IsEmpty())
			{
				Label = FText::Format(FText::FromString(TEXT("{0} ({1}{2}{3})")),
					Label,
					ExponentStr,
					UnitSpace,
					UnitStr
					);
			}
		}

		auto Extents = FontMeasureService->Measure(Label, LabelFont);
		auto AvailableSize = YAxisLabelGeometry.GetLocalSize();

		auto const RenderTransform = TransformCast< FSlateRenderTransform >(Concatenate(Inverse(FVector2D(Extents.X, 0.0f)), FQuat2D(FMath::DegreesToRadians(-90.0f))));
		YAxisLabelGeometry = YAxisLabelGeometry.MakeChild(
			Extents,
			FSlateLayoutTransform(FVector2D(0.0f, (AvailableSize.Y - Extents.X) * 0.5f)),
			RenderTransform,
			FVector2D(0.0f, 0.0f)
			);
		FSlateRect BaseClipRect(
			YAxisLabelGeometry.GetAccumulatedLayoutTransform().TransformPoint(FVector2D(0, 0)),
			YAxisLabelGeometry.GetAccumulatedLayoutTransform().TransformPoint(FVector2D(Extents.Y, Extents.X))
			);
		auto IntersectedClipRect = BaseClipRect.IntersectionWith(ClipRect);
		auto TransformedClipRect = TransformRect(
			Concatenate(
				Inverse(YAxisLabelGeometry.GetAccumulatedLayoutTransform()),
				Inverse(RenderTransform),
				YAxisLabelGeometry.GetAccumulatedLayoutTransform()
			),
			FSlateRotatedRect2(IntersectedClipRect)
			).ToBoundingRect();
		auto FinalClipRect = TransformedClipRect;
		FSlateDrawElement::MakeText(
			OutDrawElements,
			LayerId,
			YAxisLabelGeometry.ToPaintGeometry(),
			Label,
			LabelFont,
			FinalClipRect,
			ESlateDrawEffect::None,
			ChartStyle->FontColor);
	}

	return LayerId;
}
*/

void SKantanCartesianChart::GetPointsToDraw(
	TArray< FKantanCartesianDatapoint > const& InPoints,
	FCartesianAxisRange const& RangeX,
	FCartesianAxisRange const& RangeY,
	TArray< FVector2D >& OutPoints) const
{
	// Cull points outside the plot range
	
	// @TODO: Not ideal, using arbitrary inflation amount to avoid culling points which are outside
	// range but may be partially visible. To do this correctly would need knowledge of plot geometry size.
	auto XCullRange = RangeX.Normalized();
	XCullRange.Expand(1.1f);
	auto YCullRange = RangeY.Normalized();
	YCullRange.Expand(1.1f);

	auto const Count = InPoints.Num();
	OutPoints.Reset(Count);
	for (auto const& Pnt : InPoints)
	{
		if (
			Pnt.Coords.X >= XCullRange.Min &&
			Pnt.Coords.X <= XCullRange.Max &&
			Pnt.Coords.Y >= YCullRange.Min &&
			Pnt.Coords.Y <= YCullRange.Max
			)
		{
			OutPoints.Add(Pnt.Coords);
		}
	}
}

void SKantanCartesianChart::GetLinePointsToDraw(
	TArray< FKantanCartesianDatapoint > const& InPoints,
	FCartesianAxisRange const& RangeX,
	FCartesianAxisRange const& RangeY,
	TArray< FVector2D >& OutPoints) const
{
	// @TODO: Can't cull so easily - would need to output multiple line batches, and cull based on whether
	// consecutive points are outside the range on the same side (eg. both > RangeY.Max)

	auto const Count = InPoints.Num();
	OutPoints.SetNumUninitialized(Count);
	for (int32 Idx = 0; Idx < Count; ++Idx)
	{
		OutPoints[Idx] = InPoints[Idx].Coords;
	}
}

int32 SKantanCartesianChart::DrawPoints(const FGeometry& PlotSpaceGeometry, const FSlateRect& ClipRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, FName const& SeriesId, TArray< FKantanCartesianDatapoint > const& Points, FKantanSeriesStyle const& SeriesStyle) const
{
	++LayerId;

	const EKantanDataPointSize::Type DP_SizeType = DataPointSize;
	const int32 DP_PixelSize = KantanDataPointPixelSizes[DP_SizeType];

	auto const CartesianToPlotXform = CartesianToPlotTransform(PlotSpaceGeometry);
	auto const Transform = Concatenate(
		CartesianToPlotXform,
		PlotSpaceGeometry.GetAccumulatedRenderTransform()
		);
	auto RangeX = PlotScale.GetXRange(PlotSpaceGeometry.GetLocalSize());
	auto RangeY = PlotScale.GetYRange(PlotSpaceGeometry.GetLocalSize());

	TArray< FVector2D > DrawPoints;
	GetPointsToDraw(Points, RangeX, RangeY, DrawPoints);
	for (auto& Pnt : DrawPoints)
	{
		Pnt = Transform.TransformPoint(Pnt);

		// Offset half of the tile size, so the point image is drawn centered on its coordinates
		Pnt -= FVector2D(DP_PixelSize * 0.5f, DP_PixelSize * 0.5f);

		// @NOTE: This seems to help avoid an infrequent issue with the tile being stretched/distorted slightly, however believe have still seen one distortion after adding this...
		Pnt = FVector2D(
			FMath::TruncToFloat(Pnt.X),
			FMath::TruncToFloat(Pnt.Y)
			);
	}

	FIntRect CanvasRect(
		0,
		0,
		// @TODO: Hack, since we are rendering our tile later in absolute coords, seems that whatever we return from
		// FSimpleRenderTarget::GetSizeXY needs to encompass our drawing area wrt absolute origin...
		FMath::TruncToInt(FMath::Max(0.0f, ClipRect.Right)),
		FMath::TruncToInt(FMath::Max(0.0f, ClipRect.Bottom))
		);
	FIntRect ClippingRect(
		FMath::TruncToInt(FMath::Max(0.0f, ClipRect.Left)),
		FMath::TruncToInt(FMath::Max(0.0f, ClipRect.Top)),
		FMath::TruncToInt(FMath::Max(0.0f, ClipRect.Right)),
		FMath::TruncToInt(FMath::Max(0.0f, ClipRect.Bottom))
		);

	FVector2D UV0(0.0f, 0.0f);
	FVector2D UV1(1.0f, 1.0f);
	if (SeriesStyle.HasValidPointStyle())
	{
		auto PointStyle = SeriesStyle.PointStyle;
		FVector2D TextureSize = FVector2D(
			PointStyle->DataPointTexture->GetSizeX(),
			PointStyle->DataPointTexture->GetSizeY()
			);

		UV0.Set(
			PointStyle->PointSizeTextureOffsets[DP_SizeType].X / TextureSize.X,
			PointStyle->PointSizeTextureOffsets[DP_SizeType].Y / TextureSize.Y
			);
		UV1.Set(
			(PointStyle->PointSizeTextureOffsets[DP_SizeType].X + DP_PixelSize) / TextureSize.X,
			(PointStyle->PointSizeTextureOffsets[DP_SizeType].Y + DP_PixelSize) / TextureSize.Y
			);
	}

	auto ChartStyle = GetChartStyle();

	auto& Element = SeriesElements.FindChecked(SeriesId);
	if (Element->BeginRenderingCanvas(
		CanvasRect,
		ClippingRect,
		SeriesStyle.HasValidPointStyle() ? SeriesStyle.PointStyle->DataPointTexture : nullptr,
		UV0,
		UV1,
		SeriesStyle.Color * FLinearColor(1, 1, 1, ChartStyle->DataOpacity),
		FVector2D(DP_PixelSize, DP_PixelSize),
		MoveTemp(DrawPoints)))
	{
		FSlateDrawElement::MakeCustom(OutDrawElements, LayerId, Element);
	}

	return LayerId;
}

int32 SKantanCartesianChart::DrawLines(const FGeometry& PlotSpaceGeometry, const FSlateRect& ClipRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, FName const& SeriesId, TArray< FKantanCartesianDatapoint > const& Points, FKantanSeriesStyle const& SeriesStyle) const
{
	++LayerId;

	auto CartesianToPlotXform = CartesianToPlotTransform(PlotSpaceGeometry);
	auto RangeX = PlotScale.GetXRange(PlotSpaceGeometry.GetLocalSize());
	auto RangeY = PlotScale.GetYRange(PlotSpaceGeometry.GetLocalSize());

	TArray< FVector2D > DrawPoints;
	GetLinePointsToDraw(Points, RangeX, RangeY, DrawPoints);
	for (auto& Pnt : DrawPoints)
	{
		// @TODO: If transform points like this and draw via plot space geometry, all works.
		// If leave untransformed and draw via cartesian space geometry, get bizarre clipping behaviour.
		// Suspect is related to there being a render transform component to the mapping between plot space
		// and cartesian space. Still seems a little strange though given that clip rect is apparently given
		// in absolute coords, would expect that if the element is drawn in the correct place, it would be
		// clipped correctly too.
		Pnt = CartesianToPlotXform.TransformPoint(Pnt);
		//
	}

	auto ChartStyle = GetChartStyle();

	// @TODO: Drawing individual segments in attempt to workaround antialiasing issue
	TArray< FVector2D > SegmentPoints;
	SegmentPoints.SetNumUninitialized(2);

	for (int32 Idx = 0; Idx < DrawPoints.Num() - 1; ++Idx)
	{
		SegmentPoints[0] = DrawPoints[Idx];
		SegmentPoints[1] = DrawPoints[Idx + 1];

		FSlateDrawElement::MakeLines(
			OutDrawElements,
			LayerId,
			PlotSpaceGeometry.ToPaintGeometry(),
			SegmentPoints,//DrawPoints,
			ClipRect.ExtendBy(ChartClipRectExtension),
			ESlateDrawEffect::None,
			SeriesStyle.Color * FLinearColor(1, 1, 1, ChartStyle->DataOpacity),
			bAntialiasDataLines
			);
	}

	return LayerId;
}

int32 SKantanCartesianChart::DrawSeries(const FGeometry& PlotSpaceGeometry, const FSlateRect& ClipRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, FName const& SeriesId, TArray< FKantanCartesianDatapoint > const& Points, FKantanSeriesStyle const& SeriesStyle) const
{
	if (SeriesConfig[SeriesId].bDrawLines)
	{
		LayerId = DrawLines(PlotSpaceGeometry, ClipRect, OutDrawElements, LayerId, SeriesId, Points, SeriesStyle);
	}
	if (SeriesConfig[SeriesId].bDrawPoints)
	{
		LayerId = DrawPoints(PlotSpaceGeometry, ClipRect, OutDrawElements, LayerId, SeriesId, Points, SeriesStyle);
	}
	return LayerId;
}

int32 SKantanCartesianChart::DrawAxes(const FGeometry& PlotSpaceGeometry, const FSlateRect& ClipRect, FSlateWindowElementList& OutDrawElements, int32 AxisLayerId, int32 LabelLayerId, FPlotMarkerData const& MarkerData) const
{
	auto ChartStyle = GetChartStyle();
	auto const CartesianToPlotXform = CartesianToPlotTransform(PlotSpaceGeometry);

	auto const LocalOrigin = CartesianToPlotXform.TransformPoint(FVector2D::ZeroVector);
	auto const PlotSize = PlotSpaceGeometry.GetLocalSize();

	// Determine cartesian range covered by plot space
	auto CartesianRangeMin = ::Inverse(CartesianToPlotXform).TransformPoint(FVector2D::ZeroVector);
	auto CartesianRangeMax = ::Inverse(CartesianToPlotXform).TransformPoint(PlotSpaceGeometry.GetLocalSize());
	if (CartesianRangeMin.X > CartesianRangeMax.X)
	{
		Swap(CartesianRangeMin.X, CartesianRangeMax.X);
	}
	if (CartesianRangeMin.Y > CartesianRangeMax.Y)
	{
		Swap(CartesianRangeMin.Y, CartesianRangeMax.Y);
	}

	auto FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

	FSlateFontInfo AxisMarkerFont = GetLabelFont(ChartStyle, EKantanChartLabelClass::AxisMarkerLabel);

	// Horizontal axis
	if (XAxisCfg.FloatingAxis.bEnabled)
	{
		const float Y0 = FMath::Clamp(LocalOrigin.Y, 0.0f, PlotSize.Y);
		TArray< FVector2D > Points;
		Points.Add(FVector2D(
			0.0f,
			Y0
			));
		Points.Add(FVector2D(
			PlotSize.X,
			Y0
			));

		FSlateDrawElement::MakeLines(
			OutDrawElements,
			AxisLayerId,
			PlotSpaceGeometry.ToPaintGeometry(),
			Points,
			ClipRect,
			ESlateDrawEffect::None,
			ChartStyle->ChartLineColor,
			false);

		auto XRounding = MarkerData.XAxis.RL;

		auto LabelMaxExtents = DetermineAxisValueLabelMaxExtents(EAxis::X, XAxisCfg.MaxValueDigits);
		bool bFitsBelow = LabelMaxExtents.Y < (PlotSize.Y - Y0);

		if (false)	// @TODO: if show axis multiplier and unit on axis
		{
			// Axis unit text
			FText UnitText = FText::Format(
				FText::FromString(TEXT("x{0}")),
				FText::FromString(XRounding.ExponentAsString())
				);
			auto UnitExtents = FontMeasureService->Measure(UnitText, AxisMarkerFont);
			auto UnitGeometry = PlotSpaceGeometry.MakeChild(
				UnitExtents,
				FSlateLayoutTransform(FVector2D(PlotSize.X - UnitExtents.X, Y0 - UnitExtents.Y))
				);
			FSlateDrawElement::MakeText(
				OutDrawElements,
				LabelLayerId,
				UnitGeometry.ToPaintGeometry(),
				UnitText,
				AxisMarkerFont,
				ClipRect,
				ESlateDrawEffect::None,
				ChartStyle->FontColor);
		}

		// Axis markers and labels
		auto XStart = XRounding.RoundUp(CartesianRangeMin.X);
		for (auto RoundedMarkerX = XStart; RoundedMarkerX.GetFloatValue() < CartesianRangeMax.X; ++RoundedMarkerX)
		{
			if (RoundedMarkerX.IsZero())	// @TODO: && other floating axis is drawn
			{
				continue;
			}

			auto MarkerX = RoundedMarkerX.GetFloatValue();
			auto const MarkerYOffset = bFitsBelow ? AxisMarkerLength : -AxisMarkerLength;
			auto const LabelYOffset = bFitsBelow ? 0.0f : -LabelMaxExtents.Y;

			TArray< FVector2D > Points;
			auto MarkerXPlotSpace = CartesianToPlotXform.TransformPoint(FVector2D(MarkerX, 0.0f)).X;
			Points.Add(FVector2D(
				MarkerXPlotSpace,
				Y0
				));
			Points.Add(FVector2D(
				MarkerXPlotSpace,
				Y0 + MarkerYOffset
				));

			if (XAxisCfg.FloatingAxis.bShowMarkers)
			{
				FSlateDrawElement::MakeLines(
					OutDrawElements,
					AxisLayerId,
					PlotSpaceGeometry.ToPaintGeometry(),
					Points,
					ClipRect,
					ESlateDrawEffect::None,
					ChartStyle->ChartLineColor,
					true);
			}

			if (XAxisCfg.FloatingAxis.bShowLabels)
			{
				FText UnsignedLabelText = FText::FromString(RoundedMarkerX.Abs().MultiplierAsString(MarkerData.XAxis.DisplayPower));
				auto UnsignedLabelExtents = FontMeasureService->Measure(UnsignedLabelText, AxisMarkerFont);
				FText LabelText = FText::FromString(RoundedMarkerX.MultiplierAsString(MarkerData.XAxis.DisplayPower));
				auto LabelExtents = FontMeasureService->Measure(LabelText, AxisMarkerFont);
				auto LabelGeometry = PlotSpaceGeometry.MakeChild(
					LabelExtents,
					FSlateLayoutTransform(Points[1] + FVector2D(-UnsignedLabelExtents.X * 0.5f - (LabelExtents.X - UnsignedLabelExtents.X), LabelYOffset))
					);

				// Test if the text can be fully displayed within the clip bounds, and if not, don't draw it at all
				if(!WillGeometryGetClipped(LabelGeometry, ClipRect))
				{
					FSlateDrawElement::MakeText(
						OutDrawElements,
						LabelLayerId,
						LabelGeometry.ToPaintGeometry(),
						LabelText,
						AxisMarkerFont,
						ClipRect,
						ESlateDrawEffect::None,
						ChartStyle->FontColor);
				}
			}
		}
	}

	// Vertical axis
	if (YAxisCfg.FloatingAxis.bEnabled)
	{
		const float X0 = FMath::Clamp(LocalOrigin.X, 0.0f, PlotSize.X);
		TArray< FVector2D > Points;
		Points.Add(FVector2D(
			X0,
			0.0f
			));
		Points.Add(FVector2D(
			X0,
			PlotSize.Y
			));

		FSlateDrawElement::MakeLines(
			OutDrawElements,
			AxisLayerId,
			PlotSpaceGeometry.ToPaintGeometry(),
			Points,
			ClipRect,
			ESlateDrawEffect::None,
			ChartStyle->ChartLineColor,
			false);

		auto YRounding = MarkerData.YAxis.RL;

		auto LabelMaxExtents = DetermineAxisValueLabelMaxExtents(EAxis::Y, YAxisCfg.MaxValueDigits);
		bool bFitsLeft = LabelMaxExtents.X < X0;

		if (false)
		{
			// Axis unit text
			FText UnitText = FText::Format(
				FText::FromString(TEXT("x{0}")),
				FText::FromString(YRounding.ExponentAsString())
				);
			auto UnitExtents = FontMeasureService->Measure(UnitText, AxisMarkerFont);
			auto const RenderTransform = TransformCast< FSlateRenderTransform >(Concatenate(Inverse(FVector2D(UnitExtents.X, 0.0f)), FQuat2D(FMath::DegreesToRadians(-90.0f))));
			auto UnitGeometry = PlotSpaceGeometry.MakeChild(
				UnitExtents,
				FSlateLayoutTransform(FVector2D(X0, 0)),
				RenderTransform,
				FVector2D(0.0f, 0.0f)
				);
			FSlateRect BaseUnitClipRect1(
				PlotSpaceGeometry.GetAccumulatedLayoutTransform().TransformPoint(FVector2D(X0, 0)),
				PlotSpaceGeometry.GetAccumulatedLayoutTransform().TransformPoint(FVector2D(X0 + UnitExtents.Y, UnitExtents.X))
				);
			FSlateRect BaseUnitClipRect2 =
				TransformRect(
				UnitGeometry.GetAccumulatedRenderTransform(),
				FSlateRotatedRect2(FSlateRect(
				FVector2D::ZeroVector,
				UnitGeometry.GetLocalSize()
				))
				).ToBoundingRect();
			auto IntersectedUnitClipRect1 = BaseUnitClipRect1.IntersectionWith(ClipRect);
			//		auto IntersectedUnitClipRect2 = BaseUnitClipRect2.IntersectionWith(ClipRect);
			auto PretransformUnitClipRect2 = BaseUnitClipRect2;// .IntersectionWith(ClipRect);
			auto TransformedUnitClipRect1 = TransformRect(
				Concatenate(
				Inverse(UnitGeometry.GetAccumulatedLayoutTransform()),
				Inverse(RenderTransform),
				UnitGeometry.GetAccumulatedLayoutTransform()
				),
				FSlateRotatedRect2(IntersectedUnitClipRect1)
				).ToBoundingRect();
			auto TransformedUnitClipRect2 = TransformRect(
				Concatenate(
				Inverse(UnitGeometry.GetAccumulatedLayoutTransform()),
				Inverse(RenderTransform),
				UnitGeometry.GetAccumulatedLayoutTransform()
				),
				FSlateRotatedRect2(PretransformUnitClipRect2)
				).ToBoundingRect();
			auto FinalUnitClipRect1 = TransformedUnitClipRect1;
			auto FinalUnitClipRect2 = TransformedUnitClipRect2.IntersectionWith(ClipRect);
			FSlateDrawElement::MakeText(
				OutDrawElements,
				LabelLayerId,
				UnitGeometry.ToPaintGeometry(),
				UnitText,
				AxisMarkerFont,
				FinalUnitClipRect1,
				ESlateDrawEffect::None,
				ChartStyle->FontColor);
		}

		// Axis markers and labels
		auto YStart = YRounding.RoundUp(CartesianRangeMin.Y);
		for (auto RoundedMarkerY = YStart; RoundedMarkerY.GetFloatValue() < CartesianRangeMax.Y; ++RoundedMarkerY)
		{
			if (RoundedMarkerY.IsZero())	// @TODO: && other floating axis is drawn
			{
				continue;
			}

			auto MarkerY = RoundedMarkerY.GetFloatValue();
			auto const MarkerXOffset = bFitsLeft ? -AxisMarkerLength : AxisMarkerLength;

			TArray< FVector2D > Points;
			auto MarkerYPlotSpace = CartesianToPlotXform.TransformPoint(FVector2D(0.0f, MarkerY)).Y;
			Points.Add(FVector2D(
				X0,
				MarkerYPlotSpace
				));
			Points.Add(FVector2D(
				X0 + MarkerXOffset,
				MarkerYPlotSpace
				));

			if (YAxisCfg.FloatingAxis.bShowMarkers)
			{
				FSlateDrawElement::MakeLines(
					OutDrawElements,
					AxisLayerId,
					PlotSpaceGeometry.ToPaintGeometry(),
					Points,
					ClipRect,
					ESlateDrawEffect::None,
					ChartStyle->ChartLineColor,
					true);
			}

			if (YAxisCfg.FloatingAxis.bShowLabels)
			{
				FText LabelText = FText::FromString(RoundedMarkerY.MultiplierAsString(MarkerData.YAxis.DisplayPower));
				auto LabelExtents = FontMeasureService->Measure(LabelText, AxisMarkerFont);
				auto const LabelXOffset = bFitsLeft ? -LabelExtents.X : 0.0f;
				auto LabelGeometry = PlotSpaceGeometry.MakeChild(
					LabelExtents,
					FSlateLayoutTransform(Points[1] + FVector2D(LabelXOffset, -LabelExtents.Y * 0.5f))
					);

				// Test if the text can be fully displayed within the clip bounds, and if not, don't draw it at all
				if(!WillGeometryGetClipped(LabelGeometry, ClipRect))
				{
					FSlateDrawElement::MakeText(
						OutDrawElements,
						LabelLayerId,
						LabelGeometry.ToPaintGeometry(),
						LabelText,
						AxisMarkerFont,
						ClipRect,
						ESlateDrawEffect::None,
						ChartStyle->FontColor);
				}
			}
		}
	}

	return LabelLayerId + 1;
}

float SKantanCartesianChart::GetChartAreaSize(EChartContentArea::Type Area, EAxis::Type ReqComp, FVector2D const& KnownPlotSize) const
{
	switch (Area)
	{
	case EChartContentArea::XAxisBottomTitle:
		return ReqComp == EAxis::Y && XAxisCfg.LeftBottomAxis.bEnabled && XAxisCfg.LeftBottomAxis.bShowTitle ? DetermineAxisTitleSize(XAxisCfg, EAxis::X).Y : 0.0f;
	case EChartContentArea::XAxisTopTitle:
		return ReqComp == EAxis::Y && XAxisCfg.RightTopAxis.bEnabled && XAxisCfg.RightTopAxis.bShowTitle ? DetermineAxisTitleSize(XAxisCfg, EAxis::X).Y : 0.0f;
	case EChartContentArea::YAxisLeftTitle:
		return ReqComp == EAxis::X && YAxisCfg.LeftBottomAxis.bEnabled && YAxisCfg.LeftBottomAxis.bShowTitle ? DetermineAxisTitleSize(YAxisCfg, EAxis::Y).X : 0.0f;
	case EChartContentArea::YAxisRightTitle:
		return ReqComp == EAxis::X && YAxisCfg.RightTopAxis.bEnabled && YAxisCfg.RightTopAxis.bShowTitle ? DetermineAxisTitleSize(YAxisCfg, EAxis::Y).X : 0.0f;
	case EChartContentArea::XAxisBottom:
		return ReqComp == EAxis::Y && XAxisCfg.LeftBottomAxis.bEnabled ? DetermineAxisRequiredWidth(EAxis::X, XAxisCfg.MaxValueDigits, AxisMarkerLength, AxisMarkerLabelGap) : 0.0f;
	case EChartContentArea::XAxisTop:
		return ReqComp == EAxis::Y && XAxisCfg.RightTopAxis.bEnabled ? DetermineAxisRequiredWidth(EAxis::X, XAxisCfg.MaxValueDigits, AxisMarkerLength, AxisMarkerLabelGap) : 0.0f;
	case EChartContentArea::YAxisLeft:
		return ReqComp == EAxis::X && YAxisCfg.LeftBottomAxis.bEnabled ? DetermineAxisRequiredWidth(EAxis::Y, YAxisCfg.MaxValueDigits, AxisMarkerLength, AxisMarkerLabelGap) : 0.0f;
	case EChartContentArea::YAxisRight:
		return ReqComp == EAxis::X && YAxisCfg.RightTopAxis.bEnabled ? DetermineAxisRequiredWidth(EAxis::Y, YAxisCfg.MaxValueDigits, AxisMarkerLength, AxisMarkerLabelGap) : 0.0f;

	default:
		return 0.0f;
	}
}

#if 0
void SKantanCartesianChart::MakeAreaGeometries(FGeometry const& ContentGeometry, FGeometry(&Geoms)[ChartContentAreaCount]) const
{
	auto ChartStyle = GetChartStyle();
	auto FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	auto LabelFont = GetLabelFont(ChartStyle, EKantanChartLabelClass::AxisTitle);
	auto AvailableSize = ContentGeometry.GetLocalSize();
	
	auto XTitleSize = DetermineAxisTitleSize(EAxis::X);
	auto YTitleSize = DetermineAxisTitleSize(EAxis::Y);

	auto XAxisWidth = DetermineAxisRequiredWidth(EAxis::X);
	auto YAxisWidth = DetermineAxisRequiredWidth(EAxis::Y);

	auto PlotSize = FVector2D(
		AvailableSize.X,
		AvailableSize.Y
		);
	if (YAxisCfg.LeftBottomAxis.bEnabled)
	{
		if (YAxisCfg.LeftBottomAxis.bShowTitle)
		{
			PlotSize.X -= YTitleSize.X;
		}
		PlotSize.X -= YAxisWidth;
	}
	if (YAxisCfg.RightTopAxis.bEnabled)
	{
		if (YAxisCfg.RightTopAxis.bShowTitle)
		{
			PlotSize.X -= YTitleSize.X;
		}
		PlotSize.X -= YAxisWidth;
	}
	if (XAxisCfg.LeftBottomAxis.bEnabled)
	{
		if (XAxisCfg.LeftBottomAxis.bShowTitle)
		{
			PlotSize.Y -= XTitleSize.Y;
		}
		PlotSize.Y -= XAxisWidth;
	}
	if (XAxisCfg.RightTopAxis.bEnabled)
	{
		if (XAxisCfg.RightTopAxis.bShowTitle)
		{
			PlotSize.Y -= XTitleSize.Y;
		}
		PlotSize.Y -= XAxisWidth;
	}

	FVector2D Offsets[ChartContentAreaCount];

	auto XOffset = 0.0f;

	if (YAxisCfg.LeftBottomAxis.bEnabled)
	{
		if (YAxisCfg.LeftBottomAxis.bShowTitle)
		{
			Offsets[EChartContentArea::YAxisLeftTitle].X = XOffset;
			XOffset += YTitleSize.X;
		}
		Offsets[EChartContentArea::YAxisLeft].X = XOffset;
		XOffset += YAxisWidth;
	}

	Offsets[EChartContentArea::Plot].X = XOffset;
	Offsets[EChartContentArea::XAxisBottomTitle].X = XOffset;
	Offsets[EChartContentArea::XAxisTopTitle].X = XOffset;
	Offsets[EChartContentArea::XAxisBottom].X = XOffset;
	Offsets[EChartContentArea::XAxisTop].X = XOffset;
	XOffset += PlotSize.X;

	if (YAxisCfg.RightTopAxis.bEnabled)
	{
		Offsets[EChartContentArea::YAxisRight].X = XOffset;
		XOffset += YAxisWidth;
		if (YAxisCfg.RightTopAxis.bShowTitle)
		{
			Offsets[EChartContentArea::YAxisRightTitle].X = XOffset;
			XOffset += YTitleSize.X;
		}
	}

	auto YOffset = 0.0f;

	if (XAxisCfg.RightTopAxis.bEnabled)
	{
		if (XAxisCfg.RightTopAxis.bShowTitle)
		{
			Offsets[EChartContentArea::XAxisTopTitle].Y = YOffset;
			YOffset += XTitleSize.Y;
		}
		Offsets[EChartContentArea::XAxisTop].Y = YOffset;
		YOffset += XAxisWidth;
	}

	Offsets[EChartContentArea::Plot].Y = YOffset;
	Offsets[EChartContentArea::YAxisLeftTitle].Y = YOffset;
	Offsets[EChartContentArea::YAxisRightTitle].Y = YOffset;
	Offsets[EChartContentArea::YAxisLeft].Y = YOffset;
	Offsets[EChartContentArea::YAxisRight].Y = YOffset;
	YOffset += PlotSize.Y;

	if (XAxisCfg.LeftBottomAxis.bEnabled)
	{
		Offsets[EChartContentArea::XAxisBottom].Y = YOffset;
		YOffset += XAxisWidth;
		if (XAxisCfg.LeftBottomAxis.bShowTitle)
		{
			Offsets[EChartContentArea::XAxisBottomTitle].Y = YOffset;
			YOffset += XTitleSize.Y;
		}
	}

	Geoms[EChartContentArea::YAxisLeftTitle] = ContentGeometry.MakeChild(
		FVector2D(YTitleSize.X, PlotSize.Y),
		FSlateLayoutTransform(Offsets[EChartContentArea::YAxisLeftTitle])
		);
	Geoms[EChartContentArea::YAxisRightTitle] = ContentGeometry.MakeChild(
		FVector2D(YTitleSize.X, PlotSize.Y),
		FSlateLayoutTransform(Offsets[EChartContentArea::YAxisRightTitle])
		);
	Geoms[EChartContentArea::YAxisLeft] = ContentGeometry.MakeChild(
		FVector2D(YAxisWidth, PlotSize.Y),
		FSlateLayoutTransform(Offsets[EChartContentArea::YAxisLeft])
		);
	Geoms[EChartContentArea::YAxisRight] = ContentGeometry.MakeChild(
		FVector2D(YAxisWidth, PlotSize.Y),
		FSlateLayoutTransform(Offsets[EChartContentArea::YAxisRight])
		);

	Geoms[EChartContentArea::XAxisBottomTitle] = ContentGeometry.MakeChild(
		FVector2D(PlotSize.X, XTitleSize.Y),
		FSlateLayoutTransform(Offsets[EChartContentArea::XAxisBottomTitle])
		);
	Geoms[EChartContentArea::XAxisTopTitle] = ContentGeometry.MakeChild(
		FVector2D(PlotSize.X, XTitleSize.Y),
		FSlateLayoutTransform(Offsets[EChartContentArea::XAxisTopTitle])
		);
	Geoms[EChartContentArea::XAxisBottom] = ContentGeometry.MakeChild(
		FVector2D(PlotSize.X, XAxisWidth),
		FSlateLayoutTransform(Offsets[EChartContentArea::XAxisBottom])
		);
	Geoms[EChartContentArea::XAxisTop] = ContentGeometry.MakeChild(
		FVector2D(PlotSize.X, XAxisWidth),
		FSlateLayoutTransform(Offsets[EChartContentArea::XAxisTop])
		);

	Geoms[EChartContentArea::Plot] = ContentGeometry.MakeChild(
		PlotSize,
		FSlateLayoutTransform(Offsets[EChartContentArea::Plot])
		);
}
#endif

FGeometry SKantanCartesianChart::MakeCartesianSpaceGeometry(FGeometry const& LocalGeometry) const
{
#if 0
	auto CenterRelativeLocal = LocalGeometry.MakeChild(
		LocalGeometry.GetLocalSize(),
		FSlateLayoutTransform(LocalGeometry.GetLocalSize() * 0.5f)
		);
	// Y axis should be up, so negate the Y scale @TODO: make this configurable via property
	// @NOTE: Sucks but seems layout transforms can only have uniform scales, so this has to be dealt with by
	// render transform.
	auto ScaleAdjusted = CenterRelativeLocal.MakeChild(
		CenterRelativeLocal.GetLocalSize(),
		FSlateLayoutTransform(),
		//FSlateLayoutTransform(PlotScale.X, FVector2D::ZeroVector),
		FTransform2D(FScale2D(PlotScale.X, -PlotScale.Y)),
		FVector2D::ZeroVector
		);
	return ScaleAdjusted.MakeChild(
		ScaleAdjusted.GetLocalSize(),
		FSlateLayoutTransform(-CartesianSpaceFocalPoint)
		);
#endif
	ensure(false);
	return{};
}

FSlateRenderTransform SKantanCartesianChart::CartesianToPlotTransform(FGeometry const& PlotSpaceGeometry) const
{
	return PlotScale.GetTransformFromCartesianSpace(PlotSpaceGeometry.GetLocalSize());
}

FFloatRoundingLevel SKantanCartesianChart::DetermineAxisRoundingLevel(FGeometry const& PlotSpaceGeometry, EAxis::Type Axis) const
{
	auto const AxisIdx = (Axis == EAxis::X ? 0 : 1);
	auto const CartesianToPlotXform = CartesianToPlotTransform(PlotSpaceGeometry);
	auto const CartesianToPlotAxisTransform = AxisUtil::FAxisTransform::FromTransform2D(CartesianToPlotXform, AxisIdx);

	auto const DefaultMinPlotSpaceLabelSeparation = 50.0f;
	auto MinPlotSpaceLabelSeparation = DefaultMinPlotSpaceLabelSeparation * (Axis == EAxis::X ? XAxisCfg.MarkerSpacing : YAxisCfg.MarkerSpacing);
	MinPlotSpaceLabelSeparation = FMath::Max(MinPlotSpaceLabelSeparation, 1.0f);

	return AxisUtil::DetermineAxisRoundingLevel(CartesianToPlotAxisTransform, MinPlotSpaceLabelSeparation);
}

AxisUtil::FAxisMarkerData SKantanCartesianChart::DetermineAxisMarkerData(FGeometry const& PlotSpaceGeometry, EAxis::Type Axis) const
{
	auto const AxisIdx = Axis == EAxis::X ? 0 : 1;

	auto CartesianToPlotXform = CartesianToPlotTransform(PlotSpaceGeometry);
	auto CartesianRangeMin = ::Inverse(CartesianToPlotXform).TransformPoint(FVector2D::ZeroVector);
	auto CartesianRangeMax = ::Inverse(CartesianToPlotXform).TransformPoint(PlotSpaceGeometry.GetLocalSize());
	auto AxisCartesianRange = FCartesianAxisRange(CartesianRangeMin[AxisIdx], CartesianRangeMax[AxisIdx]).Normalized();
	
	auto const& AxisCfg = Axis == EAxis::X ? XAxisCfg : YAxisCfg;

	return AxisUtil::DetermineAxisMarkerData(
		DetermineAxisRoundingLevel(PlotSpaceGeometry, Axis),
		AxisCartesianRange,
		AxisCfg.MaxValueDigits
		);
}

void SKantanCartesianChart::AddReferencedObjects(FReferenceCollector& Collector)
{
	if (Datasource)
	{
		Collector.AddReferencedObject(Datasource);
	}
}


FDataSeriesElement::FDataSeriesElement() : RenderTarget(MakeUnique< FSimpleRenderTarget >())
{}

FDataSeriesElement::~FDataSeriesElement()
{}

bool FDataSeriesElement::BeginRenderingCanvas(const FIntRect& InCanvasRect, const FIntRect& InClippingRect, /*FSlateRenderTransform const& RenderTransform,*/ UTexture2D* InTexture, FVector2D InUV_0, FVector2D InUV_1, FLinearColor InColor, FVector2D InPointSize, TArray< FVector2D >&& InPoints)
{
	if (InCanvasRect.Size().X > 0 && InCanvasRect.Size().Y > 0 && InClippingRect.Size().X > 0 && InClippingRect.Size().Y > 0 && InPoints.Num() > 0)
	{
		/**
		* Struct to contain all info that needs to be passed to the render thread
		*/
		struct FSeriesRenderInfo
		{
			/** Size of the Canvas tile */
			FIntRect CanvasRect;
			/** How to clip the canvas tile */
			FIntRect ClippingRect;

			TSharedPtr< FRenderData > RenderData;
//			FSlateRenderTransform RenderTransform;
		};

		FSeriesRenderInfo RenderInfo;
		RenderInfo.CanvasRect = InCanvasRect;
		RenderInfo.ClippingRect = InClippingRect;
		RenderInfo.RenderData = MakeShareable< FRenderData >(new FRenderData);
		RenderInfo.RenderData->TextureResource = InTexture ? InTexture->Resource : GWhiteTexture;
		RenderInfo.RenderData->UV_0 = InUV_0;
		RenderInfo.RenderData->UV_1 = InUV_1;
		RenderInfo.RenderData->Color = InColor;
		RenderInfo.RenderData->PointSize = InPointSize;
		RenderInfo.RenderData->Points = MoveTemp(InPoints);
//		RenderInfo.RenderTransform = RenderTransform;

		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER
			(
			BeginRenderingKantanCartesianSeriesCanvas,
			FDataSeriesElement*, SeriesElement, this,
			FSeriesRenderInfo, InRenderInfo, MoveTemp(RenderInfo),
			{
				SeriesElement->RenderTarget->SetViewRect(InRenderInfo.CanvasRect);
				SeriesElement->RenderTarget->SetClippingRect(InRenderInfo.ClippingRect);
				//SeriesElement->RenderTarget->SetRenderTransform(InRenderInfo.RenderTransform);
				SeriesElement->RenderData = MoveTemp(*InRenderInfo.RenderData);
			}
		);
		return true;
	}

	return false;
}

void FDataSeriesElement::DrawRenderThread(FRHICommandListImmediate& RHICmdList, const void* InWindowBackBuffer)
{
	FIntRect ViewRect = RenderTarget->GetViewRect();
	// Clip the canvas to avoid having to set UV values
	FIntRect ClippingRect = RenderTarget->GetClippingRect();

//	auto RenderTransform = RenderTarget->GetRenderTransform();

	RHICmdList.SetScissorRect(
		true,
		ClippingRect.Min.X,
		ClippingRect.Min.Y,
		ClippingRect.Max.X,
		ClippingRect.Max.Y
		);
	RenderTarget->SetRenderTargetTexture(*(FTexture2DRHIRef*)InWindowBackBuffer);
	{
		// Check realtime mode for whether to pass current time to canvas
		float CurrentTime = FApp::GetCurrentTime() - GStartTime;
		float DeltaTime = FApp::GetDeltaTime();

		FCanvas Canvas(RenderTarget.Get(), nullptr, CurrentTime, CurrentTime, DeltaTime, GMaxRHIFeatureLevel);
		{
			Canvas.SetAllowedModes(0);
			bool bTestIsScaled = Canvas.IsScaledToRenderTarget();
			Canvas.SetScaledToRenderTarget(false);
			//Canvas.SetRenderTargetRect(//RenderTarget->GetViewRect());

/*			auto RenderTransform2D = RenderTarget->GetRenderTransform();
			auto RotationScale2D = RenderTransform2D.GetMatrix();
			auto Translation2D = RenderTransform2D.GetTranslation();
			FMatrix RenderTransformMatrix = FMatrix::Identity;
			RotationScale2D.GetMatrix(
				RenderTransformMatrix.M[0][0],
				RenderTransformMatrix.M[0][1],
				RenderTransformMatrix.M[1][0],
				RenderTransformMatrix.M[1][1]
				);
			RenderTransformMatrix.M[3][0] = Translation2D.X;
			RenderTransformMatrix.M[3][1] = Translation2D.Y;
			Canvas.PushRelativeTransform(RenderTransformMatrix);
*/
			FCanvasTileItem Tile(
				FVector2D(),
				RenderData.TextureResource,
				RenderData.PointSize,
				RenderData.UV_0,
				RenderData.UV_1,
				RenderData.Color);
			Tile.BlendMode = ESimpleElementBlendMode::SE_BLEND_Masked;
			for (auto const& Pnt : RenderData.Points)
			{
				Tile.Position = Pnt;
				Canvas.DrawItem(Tile);
			}

//			Canvas.PopTransform();
		}
		Canvas.Flush_RenderThread(RHICmdList, true);
	}
	RenderTarget->ClearRenderTargetTexture();
	
	RHICmdList.SetScissorRect(false, 0, 0, 0, 0);
}


