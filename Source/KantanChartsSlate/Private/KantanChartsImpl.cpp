// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "KantanChartsImpl.h"
#include "Charts/SKantanCartesianChart.h"
#include "Charts/SKantanTimeSeriesPlot.h"
#include "Charts/SKantanBarChart.h"
#include "Charts/SKantanChartLegend.h"
#include "Style/KantanChartsStyleSet.h"
#include "KantanSimpleCartesianDatasource.h"
#include "KantanSimpleCategoryDatasource.h"


namespace KantanCharts {

	TSharedRef< ICartesianChart > FKantanChartsImpl::CreateCartesianChart() const
	{
		return SNew(SKantanCartesianChart);
	}

	TSharedRef< ITimeSeriesChart > FKantanChartsImpl::CreateTimeSeriesChart() const
	{
		return SNew(SKantanTimeSeriesPlot);
	}

	TSharedRef< IBarChart > FKantanChartsImpl::CreateBarChart() const
	{
		return SNew(SKantanBarChart);
	}

	TSharedRef< IChartLegend > FKantanChartsImpl::CreateCartesianLegend() const
	{
		return SNew(SKantanChartLegend);
	}

	UKantanSimpleCartesianDatasource* FKantanChartsImpl::CreateSimpleCartesianDatasource(UObject* Outer) const
	{
		return NewObject< UKantanSimpleCartesianDatasource >(Outer);
	}

	UKantanSimpleCategoryDatasource* FKantanChartsImpl::CreateSimpleCategoryDatasource(UObject* Outer) const
	{
		return NewObject< UKantanSimpleCategoryDatasource >(Outer);
	}

	const FKantanCartesianChartStyle& FKantanChartsImpl::GetDefaultCartesianChartEditorStyle() const
	{
		return FKantanChartsStyleSet::Get().GetWidgetStyle< FKantanCartesianChartStyle >(TEXT("CartesianChart.DefaultEditor"));
	}

	const FKantanBarChartStyle& FKantanChartsImpl::GetDefaultBarChartEditorStyle() const
	{
		return FKantanChartsStyleSet::Get().GetWidgetStyle< FKantanBarChartStyle >(TEXT("BarChart.DefaultEditor"));
	}

	const FKantanCartesianChartStyle& FKantanChartsImpl::GetDefaultCartesianChartGameStyle() const
	{
		return FKantanChartsStyleSet::Get().GetWidgetStyle< FKantanCartesianChartStyle >(TEXT("CartesianChart.DefaultGame"));
	}

	const FKantanBarChartStyle& FKantanChartsImpl::GetDefaultBarChartGameStyle() const
	{
		return FKantanChartsStyleSet::Get().GetWidgetStyle< FKantanBarChartStyle >(TEXT("BarChart.DefaultGame"));
	}

	FName FKantanChartsImpl::GetEditorStyleSetName() const
	{
		// @TODO: Update if change to separate editor and runtime style sets
		return FKantanChartsStyleSet::GetStyleSetName();
	}

}


