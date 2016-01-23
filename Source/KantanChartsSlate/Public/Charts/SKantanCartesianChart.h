// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

#pragma once

#include "SKantanChart.h"
#include "KantanCartesianTypes.h"
#include "CartesianAxisConfig.h"
#include "KantanCartesianDatapoint.h"
#include "CartesianData.h"
#include "KantanSeriesStyle.h"
#include "KantanCartesianChartStyle.h"
#include "KantanChartsStyleSet.h"
#include "FloatRoundingLevel.h"
#include "AxisUtility.h"


/** A widget class implementing a cartesian chart. */
class KANTANCHARTSSLATE_API SKantanCartesianChart : public SKantanChart
{
public:
	~SKantanCartesianChart();

	SLATE_BEGIN_ARGS(SKantanCartesianChart):
		_Style(&FKantanChartsStyleSet::Get().GetWidgetStyle< FKantanCartesianChartStyle >("KantanCartesianChartStyle_Default"))
		, _Datasource(nullptr)
		, _UpdateTickRate(0.0f)
		, _PlotScale()
		, _DataPointSize(EKantanDataPointSize::Medium)
		{}

		SLATE_STYLE_ARGUMENT(FKantanCartesianChartStyle, Style)

		/** Rate at which this widget is ticked when sleeping in seconds */
		SLATE_ARGUMENT(float, UpdateTickRate)
		SLATE_ARGUMENT(class UObject*, Datasource)

		SLATE_ARGUMENT(FKantanCartesianPlotScale, PlotScale)

		SLATE_ARGUMENT(EKantanDataPointSize::Type, DataPointSize)

	SLATE_END_ARGS()

	/**
	 * Construct the widget
	 * 
	 * @param InArgs   A declaration from which to construct the widget
	 */
	void Construct(const FArguments& InArgs);

	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	void SetStyle(const FKantanCartesianChartStyle* InStyle);
	bool SetDatasource(UObject* InDatasource);
	void SetUseAutoPerSeriesStyles(bool bEnabled);
	void SetSeriesStylesList(TArray< FKantanSeriesStyle > const& Styles);
	void SetManualSeriesStyleMappings(TMap< FName, FName > const& Mappings);
	void SetPlotScale(FKantanCartesianPlotScale const& Scaling);
	void SetDataPointSize(EKantanDataPointSize::Type InSize);
	void SetXAxisConfig(FCartesianAxisConfig const& InConfig);
	void SetYAxisConfig(FCartesianAxisConfig const& InConfig);
	void SetAxisTitlePadding(FMargin const& InPadding);
	void SetAntialiasDataLines(bool bEnable);

	void EnableSeries(FName Id, bool bEnable);
	void ConfigureSeries(FName Id, bool bDrawPoints, bool bDrawLines);
	void SetSeriesStyle(FName Id, FName StyleId);
	void ResetSeries(FName Id = NAME_None);

	bool IsSeriesEnabled(FName Id) const;
	bool IsSeriesShowingLines(FName Id) const;
	bool IsSeriesShowingPoints(FName Id) const;

	int32 GetNumSeries() const;
	FName GetSeriesId(int32 Index) const;
	FText GetSeriesLabel(int32 Index) const;
	TArray< FKantanCartesianDatapoint > GetSeriesDatapoints(int32 Index) const;

	virtual const FKantanCartesianChartStyle* GetChartStyle() const override
	{
		return static_cast<const FKantanCartesianChartStyle*>(Style);
	}

	FKantanSeriesStyle const& GetSeriesStyle(FName SeriesId) const;

	FCartesianDataSnapshot const& GetCurrentSnapshot() const
	{
		return DataSnapshot;
	}

	static bool IsValidDatasource(UObject* Source);
	static bool IsNullOrValidDatasource(UObject* Source);

protected:
	struct FPlotMarkerData
	{
		AxisUtil::FAxisMarkerData XAxis;
		AxisUtil::FAxisMarkerData YAxis;
	};

protected:
	// Create geometry that maps to the actual cartesian space on which we are plotting points
	FGeometry MakeCartesianSpaceGeometry(FGeometry const& LocalGeometry) const;

	// @TODO: Making use of LocalGeometry seems weird, since this transform should be independent of it
	FSlateRenderTransform CartesianToPlotTransform(FGeometry const& LocalGeometry) const;

	// Returns the interval between markers along each axis, in cartesian space
	class FFloatRoundingLevel DetermineAxisRoundingLevel(FGeometry const& PlotSpaceGeometry, EAxis::Type Axis) const;

	AxisUtil::FAxisMarkerData DetermineAxisMarkerData(FGeometry const& PlotSpaceGeometry, EAxis::Type Axis) const;

	inline const FKantanSeriesStyle* FindSeriesStyle(FName SeriesStyleId) const
	{
		return SeriesStyles.FindByPredicate([SeriesStyleId](FKantanSeriesStyle const& Style)
		{
			return Style.StyleId == SeriesStyleId;
		});
	}

	void UpdateDrawingElementsFromDatasource();
	void UpdateSeriesConfigFromDatasource();
	FName GetNextSeriesStyle() const;

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

//	int32 DrawAxisLabels(const FGeometry& ContentGeometry, const FSlateRect& ClipRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, FPlotMarkerData const& MarkerData) const;
	int32 DrawAxes(const FGeometry& PlotSpaceGeometry, const FSlateRect& ClipRect, FSlateWindowElementList& OutDrawElements, int32 AxisLayerId, int32 LabelLayerId, FPlotMarkerData const& MarkerData) const;
	int32 DrawPoints(const FGeometry& PlotSpaceGeometry, const FSlateRect& ClipRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, FName const& SeriesId, TArray< FKantanCartesianDatapoint > const& Points, FKantanSeriesStyle const& SeriesStyle) const;
	int32 DrawLines(const FGeometry& PlotSpaceGeometry, const FSlateRect& ClipRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, FName const& SeriesId, TArray< FKantanCartesianDatapoint > const& Points, FKantanSeriesStyle const& SeriesStyle) const;
	int32 DrawSeries(const FGeometry& PlotSpaceGeometry, const FSlateRect& ClipRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, FName const& SeriesId, TArray< FKantanCartesianDatapoint > const& Points, FKantanSeriesStyle const& SeriesStyle) const;

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
	EKantanDataPointSize::Type DataPointSize;
	bool bAntialiasDataLines;

	// Data
	UObject* Datasource;
	FCartesianDataSnapshot DataSnapshot;

	// Chart style
	const FKantanCartesianChartStyle* Style;

	// Series style
	bool bAutoPerSeriesStyles;
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

	// Implementation
	typedef TSharedPtr< class FDataSeriesElement, ESPMode::ThreadSafe > FSeriesElemPtr;
	TMap< FName, FSeriesElemPtr > SeriesElements;
};

