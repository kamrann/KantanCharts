// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "Charts/SKantanChart.h"
#include "KantanCartesianTypes.h"
#include "Charts/CartesianAxisConfig.h"
#include "KantanCartesianDatapoint.h"
#include "CartesianData.h"
#include "Style/KantanSeriesStyle.h"
#include "Style/KantanCartesianChartStyle.h"
#include "Style/KantanChartsStyleSet.h"
#include "FloatRoundingLevel.h"
#include "AxisUtility.h"
#include "Interfaces/ICartesianChart.h"

#include "Styling/SlateWidgetStyleAsset.h"


/** A widget class implementing a cartesian chart. */
class KANTANCHARTSSLATE_API SKantanCartesianChart:
	public SKantanChart
	, public virtual KantanCharts::ICartesianChart
{
public:
	~SKantanCartesianChart();

	SLATE_BEGIN_ARGS(SKantanCartesianChart):
		_Style(&FKantanChartsStyleSet::Get().GetWidgetStyle< FKantanCartesianChartStyle >("CartesianChart.DefaultGame"))
		, _Datasource(nullptr)
		, _UpdateTickRate(0.0f)
		, _PlotScale()
		, _DataPointSize(EKantanDataPointSize::Medium)
		, _OnUpdatePlotScale()
		{}

		SLATE_STYLE_ARGUMENT(FKantanCartesianChartStyle, Style)

		SLATE_ARGUMENT(class UObject*, Datasource)
		
		/** Rate at which this widget is ticked when sleeping in seconds */
		SLATE_ARGUMENT(float, UpdateTickRate)

		SLATE_ARGUMENT(FKantanCartesianPlotScale, PlotScale)

		SLATE_ARGUMENT(EKantanDataPointSize::Type, DataPointSize)

		SLATE_EVENT(FOnUpdatePlotScale, OnUpdatePlotScale)

	SLATE_END_ARGS()

	/**
	 * Construct the widget
	 * 
	 * @param InArgs   A declaration from which to construct the widget
	 */
	void Construct(const FArguments& InArgs);

	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	virtual void SetStyle(const FKantanCartesianChartStyle* InStyle) override;
	virtual void SetStyleFromAsset(USlateWidgetStyleContainerBase* InStyleAsset) override;
	virtual bool SetDatasource(UObject* InDatasource) override;
	virtual void SetUseAutoPerSeriesStyles(bool bEnabled) override;
	virtual void SetSeriesStylesList(TArray< FKantanSeriesStyle > const& Styles) override;
	virtual void LoadSeriesStylesList(const FSoftObjectPath& Styles) override;
	virtual void SetManualSeriesStyleMappings(TMap< FName, FName > const& Mappings) override;
	virtual void SetPlotScale(FKantanCartesianPlotScale const& Scaling) override;
	virtual void SetDataPointSize(EKantanDataPointSize::Type InSize) override;
	virtual void SetXAxisConfig(FCartesianAxisConfig const& InConfig) override;
	virtual void SetYAxisConfig(FCartesianAxisConfig const& InConfig) override;
	virtual void SetAxisTitlePadding(FMargin const& InPadding) override;
	virtual void SetAntialiasDataLines(bool bEnable) override;
	virtual void SetOnUpdatePlotScale(FOnUpdatePlotScale Delegate) override;

	virtual void EnableSeries(FName Id, bool bEnable) override;
	virtual void ConfigureSeries(FName Id, bool bDrawPoints, bool bDrawLines) override;
	virtual void SetSeriesStyle(FName Id, FName StyleId) override;
	virtual void ResetSeries(FName Id = NAME_None) override;
	//virtual void ResetSeriesNotInDatasource() override;

	virtual bool IsSeriesEnabled(FName Id) const override;
	virtual bool IsSeriesShowingLines(FName Id) const override;
	virtual bool IsSeriesShowingPoints(FName Id) const override;

	int32 GetNumSeries() const;
	FName GetSeriesId(int32 Index) const;
	FText GetSeriesLabel(int32 Index) const;
	TArray< FKantanCartesianDatapoint > GetSeriesDatapoints(int32 Index) const;

	virtual const FKantanCartesianChartStyle* GetChartStyle() const override
	{
		return static_cast<const FKantanCartesianChartStyle*>(Style);
	}

	virtual FKantanSeriesStyle const& GetSeriesStyle(FName SeriesId) const override;

	virtual FCartesianDataSnapshot const& GetCurrentSnapshot() const override
	{
		return DataSnapshot;
	}

	virtual TSharedRef< SWidget > AsWidget() override { return SKantanChart::AsWidget(); }

	virtual void SetMargins(FMargin const& InMargins) override { SKantanChart::SetMargins(InMargins); }
	virtual void SetChartTitle(FText const& InTitle) override { SKantanChart::SetChartTitle(InTitle); }
	virtual void SetChartTitlePadding(FMargin const& InPadding) override { SKantanChart::SetChartTitlePadding(InPadding); }
	virtual void SetUpdateTickRate(float InRate) override { SKantanChart::SetUpdateTickRate(InRate); }
	virtual void SetOnChartMouseDown(KantanCharts::FOnInteractionMouseDown InDelegate) override { SKantanChart::SetOnChartMouseDown(InDelegate); }

	static bool IsValidDatasource(UObject* Source);
	static bool IsNullOrValidDatasource(UObject* Source);

protected:
	struct FPlotMarkerData
	{
		AxisUtil::FAxisMarkerData XAxis;
		AxisUtil::FAxisMarkerData YAxis;
	};

protected:
	// @TODO: Making use of LocalGeometry seems weird, since this transform should be independent of it
	FSlateRenderTransform CartesianToPlotTransform(FGeometry const& LocalGeometry) const;

	// Returns the interval between markers along each axis, in cartesian space
	class FFloatRoundingLevel DetermineAxisRoundingLevel(FGeometry const& PlotSpaceGeometry, EAxis::Type Axis) const;

	AxisUtil::FAxisMarkerData DetermineAxisMarkerData(FGeometry const& PlotSpaceGeometry, EAxis::Type Axis) const;

	inline const FKantanSeriesStyle* FindSeriesStyle(FName SeriesStyleId) const
	{
		return SeriesStyles.FindByPredicate([SeriesStyleId](FKantanSeriesStyle const& SeriesStyle)
		{
			return SeriesStyle.StyleId == SeriesStyleId;
		});
	}

	typedef TSharedPtr< class IDataSeriesElement, ESPMode::ThreadSafe > FSeriesElemPtr;

	void DiscardDrawingElements(TArray< FSeriesElemPtr >& Elements);
	void DiscardAllDrawingElements();
	void UpdateDrawingElementsFromDatasource();
	void UpdateSeriesConfigFromDatasource();
	FName GetNextSeriesStyle() const;

	static FCartesianAxisRange ValidateAxisDisplayRange(FCartesianAxisRange InRange);

protected:
	virtual float GetChartAreaSize(EChartContentArea::Type Area, EAxis::Type ReqComp, FVector2D const& KnownPlotSize) const override;
	virtual int32 DrawChartArea(
		EChartContentArea::Type Area,
		const FPaintArgs& Args,
		const FGeometry& Geometry,
		const FGeometry& PlotSpaceGeometry,
		const FSlateRect& MyClippingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled
		) const override;
	virtual void OnActiveTick(double InCurrentTime, float InDeltaTime) override;

	// Fills an array with the points (in cartesian space) which should be drawn
	virtual void GetPointsToDraw(
		TArray< FKantanCartesianDatapoint > const& InPoints,
		FCartesianAxisRange const& RangeX,
		FCartesianAxisRange const& RangeY,
		TArray< FVector2D >& OutPoints) const;
	virtual void GetLinePointsToDraw(
		TArray< FKantanCartesianDatapoint > const& InPoints,
		FCartesianAxisRange const& RangeX,
		FCartesianAxisRange const& RangeY,
		TArray< FVector2D >& OutPoints) const;

	int32 DrawAxes(const FGeometry& PlotSpaceGeometry, const FSlateRect& ClipRect, FSlateWindowElementList& OutDrawElements, int32 AxisLayerId, int32 LabelLayerId, FPlotMarkerData const& MarkerData) const;
	int32 DrawPoints(const FGeometry& PlotSpaceGeometry, const FSlateRect& ClipRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, FName const& SeriesId, TArray< FKantanCartesianDatapoint > const& Points, FKantanSeriesStyle const& SeriesStyle) const;
	int32 DrawLines(const FGeometry& PlotSpaceGeometry, const FSlateRect& ClipRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, FName const& SeriesId, TArray< FKantanCartesianDatapoint > const& Points, FKantanSeriesStyle const& SeriesStyle) const;
	int32 DrawSeries(const FGeometry& PlotSpaceGeometry, const FSlateRect& ClipRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, FName const& SeriesId, TArray< FKantanCartesianDatapoint > const& Points, FKantanSeriesStyle const& SeriesStyle) const;

	void InvalidateCachedMarkerData(EAxis::Type Axis) const;
	const AxisUtil::FAxisMarkerData& GetCachedMarkerData(EAxis::Type Axis, FGeometry const& PlotSpaceGeometry) const;

public:
	// FGCObject implementation
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	//

protected:
	// General config
	FCartesianAxisConfig XAxisCfg;
	FCartesianAxisConfig YAxisCfg;
	FMargin AxisTitlePadding;
	FKantanCartesianPlotScale PlotScale;
	EKantanDataPointSize::Type DataPointSize = EKantanDataPointSize::Medium;
	bool bAntialiasDataLines = false;

	FOnUpdatePlotScale OnUpdatePlotScaleDelegate;

	// Data
	UObject* Datasource = nullptr;
	FCartesianDataSnapshot DataSnapshot;

	// Chart style
	const FKantanCartesianChartStyle* Style = nullptr;

	// Series style
	bool bAutoPerSeriesStyles = true;
	TArray< FKantanSeriesStyle > SeriesStyles;

	struct FSeriesConfig
	{
		bool bEnabled;			// Toggles series on/off
		bool bDrawPoints;		// Draw datapoints
		bool bDrawLines;		// Draw lines between datapoints
		FName SeriesStyleId;	// Style to use. If NAME_None, default series style is used

		FSeriesConfig() :
			bEnabled(false),
			bDrawPoints(true),
			bDrawLines(false),
			SeriesStyleId(NAME_None)
		{}
	};

	TMap< FName, FSeriesConfig > SeriesConfig;
	TMap< FName, FSeriesElemPtr > SeriesElements;

	mutable TOptional< AxisUtil::FAxisMarkerData > XAxisMarkers;
	mutable TOptional< AxisUtil::FAxisMarkerData > YAxisMarkers;
};

