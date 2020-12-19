// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "Charts/SKantanCartesianChart.h"
#include "Style/KantanChartsStyleSet.h"
#include "Interfaces/ITimeSeriesChart.h"


/** A widget class implementing a cartesian chart specialized for plotting time-varying series. */
class KANTANCHARTSSLATE_API SKantanTimeSeriesPlot:
	public SKantanCartesianChart
	, public virtual KantanCharts::ITimeSeriesChart
{
public:
	SLATE_BEGIN_ARGS(SKantanTimeSeriesPlot) :
		// @TODO: TimeSeriesPlot dedicated style
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
	virtual void SetFixedTimeRange(TOptional< float > InRange) override;
//	virtual void SetRoundTimeRange(bool bInRound) override;
	virtual void SetLowerValueBound(FCartesianRangeBound const& InBound) override;
	virtual void SetUpperValueBound(FCartesianRangeBound const& InBound) override;
	virtual void SetLowerTimeBound(FCartesianRangeBound const& InBound) override;
	virtual void SetUpperTimeBound(FCartesianRangeBound const& InBound) override;
	
	void SetExtendValueRangeToZero(bool bExtendToZero);

	// @NOTE: All the following are needed only because VC++ doesn't support the use of the 'using' keyword
	// for resolving inheritance via dominance warnings, and for some reason the pragma is failing to suppress
	// them also.
	virtual TSharedRef< SWidget > AsWidget() override { return SKantanCartesianChart::AsWidget(); }

	virtual void SetMargins(FMargin const& InMargins) override { SKantanCartesianChart::SetMargins(InMargins); }
	virtual void SetChartTitle(FText const& InTitle) override { SKantanCartesianChart::SetChartTitle(InTitle); }
	virtual void SetChartTitlePadding(FMargin const& InPadding) override { SKantanCartesianChart::SetChartTitlePadding(InPadding); }
	virtual void SetUpdateTickRate(float InRate) override { SKantanCartesianChart::SetUpdateTickRate(InRate); }
	virtual void SetOnChartMouseDown(KantanCharts::FOnInteractionMouseDown InDelegate) override { SKantanCartesianChart::SetOnChartMouseDown(InDelegate); }

	virtual const FKantanCartesianChartStyle* GetChartStyle() const override { return SKantanCartesianChart::GetChartStyle(); }

	virtual void SetStyle(const FKantanCartesianChartStyle* InStyle) override { SKantanCartesianChart::SetStyle(InStyle); }
	virtual void SetStyleFromAsset(USlateWidgetStyleContainerBase* InStyleAsset) override { SKantanCartesianChart::SetStyleFromAsset(InStyleAsset); }
	virtual bool SetDatasource(UObject* InDatasource) override { return SKantanCartesianChart::SetDatasource(InDatasource); }
	virtual void SetUseAutoPerSeriesStyles(bool bEnabled) override { SKantanCartesianChart::SetUseAutoPerSeriesStyles(bEnabled); }
	virtual void SetSeriesStylesList(TArray< FKantanSeriesStyle > const& Styles) override { SKantanCartesianChart::SetSeriesStylesList(Styles); }
	virtual void LoadSeriesStylesList(const FSoftObjectPath& Styles) override { SKantanCartesianChart::LoadSeriesStylesList(Styles); }
	virtual void SetManualSeriesStyleMappings(TMap< FName, FName > const& Mappings) override { SKantanCartesianChart::SetManualSeriesStyleMappings(Mappings); }
	virtual void SetPlotScale(FKantanCartesianPlotScale const& Scaling) override { SKantanCartesianChart::SetPlotScale(Scaling); }
	virtual void SetDataPointSize(EKantanDataPointSize::Type InSize) override { SKantanCartesianChart::SetDataPointSize(InSize); }
	virtual void SetXAxisConfig(FCartesianAxisConfig const& InConfig) override { SKantanCartesianChart::SetXAxisConfig(InConfig); }
	virtual void SetYAxisConfig(FCartesianAxisConfig const& InConfig) override { SKantanCartesianChart::SetYAxisConfig(InConfig); }
	virtual void SetAxisTitlePadding(FMargin const& InPadding) override { SKantanCartesianChart::SetAxisTitlePadding(InPadding); }
	virtual void SetAntialiasDataLines(bool bEnable) override { SKantanCartesianChart::SetAntialiasDataLines(bEnable); }
	virtual void SetOnUpdatePlotScale(FOnUpdatePlotScale Delegate) override { SKantanCartesianChart::SetOnUpdatePlotScale(Delegate); }

	virtual void EnableSeries(FName Id, bool bEnable) override { SKantanCartesianChart::EnableSeries(Id, bEnable); }
	virtual void ConfigureSeries(FName Id, bool bDrawPoints, bool bDrawLines) override { SKantanCartesianChart::ConfigureSeries(Id, bDrawPoints, bDrawLines); }
	virtual void SetSeriesStyle(FName Id, FName StyleId) override { SKantanCartesianChart::SetSeriesStyle(Id, StyleId); }
	virtual void ResetSeries(FName Id = NAME_None) override { SKantanCartesianChart::ResetSeries(Id); }

	virtual bool IsSeriesEnabled(FName Id) const override { return SKantanCartesianChart::IsSeriesEnabled(Id); }
	virtual bool IsSeriesShowingLines(FName Id) const override { return SKantanCartesianChart::IsSeriesShowingLines(Id); }
	virtual bool IsSeriesShowingPoints(FName Id) const override { return SKantanCartesianChart::IsSeriesShowingPoints(Id); }

	virtual FKantanSeriesStyle const& GetSeriesStyle(FName SeriesId) const override { return SKantanCartesianChart::GetSeriesStyle(SeriesId); }
	virtual FCartesianDataSnapshot const& GetCurrentSnapshot() const override { return SKantanCartesianChart::GetCurrentSnapshot(); }

protected:
	virtual void OnActiveTick(double InCurrentTime, float InDeltaTime) override;

	virtual void GetLinePointsToDraw(
		TArray< FKantanCartesianDatapoint > const& InPoints,
		FCartesianAxisRange const& RangeX,
		FCartesianAxisRange const& RangeY,
		TArray< FVector2D >& OutPoints) const override;

	FKantanCartesianPlotScale DeterminePlotScale(const FCartesianDataSnapshot& Snapshot, const TArray< int32 >& Enabled) const;

protected:
	FCartesianRangeBound LowerValueBound;
	FCartesianRangeBound UpperValueBound;
	FCartesianRangeBound LowerTimeBound;
	FCartesianRangeBound UpperTimeBound;
	bool bExtendValueRangeToZero = true;
	TOptional< float > FixedTimeRange;
	//bool bRoundTimeRange;
};



