// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

#pragma once

#include "SKantanCartesianChart.h"
//#include "KantanTimeSeriesPlotStyle.h"
#include "KantanChartsStyleSet.h"


/** A widget class implementing a cartesian chart specialized for plotting time-varying series. */
class KANTANCHARTSSLATE_API SKantanTimeSeriesPlot : public SKantanCartesianChart
{
public:
	SLATE_BEGIN_ARGS(SKantanTimeSeriesPlot) :
		// @TODO: TimeSeriesPlot dedicated style
		//_Style(&FKantanChartsStyleSet::Get().GetWidgetStyle< FKantanCartesianChartStyle >("ChartStyles/KantanCartesianChartStyle_Default"))
		_Style(&FKantanChartsStyleSet::Get().GetWidgetStyle< FKantanCartesianChartStyle >("CartesianChart.DefaultGame"))
		, _Datasource(nullptr)
		, _UpdateTickRate(0.0f)
		{}

		SLATE_STYLE_ARGUMENT(FKantanCartesianChartStyle, Style)

		SLATE_ARGUMENT(class UObject*, Datasource)

		/** Rate at which this widget is ticked when sleeping in seconds */
		SLATE_ARGUMENT(float, UpdateTickRate)

	SLATE_END_ARGS()

		/**
		 * Construct the widget
		 *
		 * @param InArgs   A declaration from which to construct the widget
		 */
	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

//	void SetStyle(const FKantanCartesianChartStyle* InStyle);
	void SetDisplayTimeRange(float InRange);
	void SetRoundTimeRange(bool bInRound);
	void SetLowerValueBound(FCartesianRangeBound const& InBound);
	void SetUpperValueBound(FCartesianRangeBound const& InBound);

protected:
	virtual void OnActiveTick(double InCurrentTime, float InDeltaTime) override;

	virtual void GetLinePointsToDraw(
		TArray< FKantanCartesianDatapoint > const& InPoints,
		FCartesianAxisRange const& RangeX,
		FCartesianAxisRange const& RangeY,
		TArray< FVector2D >& OutPoints) const override;

	// @TODO: Integrate this into datasource
	FCartesianAxisRange GetDataAxisSpan(EAxis::Type Axis) const;
	static FCartesianAxisRange ValidateAxisDisplayRange(FCartesianAxisRange InRange);

protected:
	float DisplayTimeRange;
	bool bRoundTimeRange;
	FCartesianRangeBound LowerValueBound;
	FCartesianRangeBound UpperValueBound;
};



