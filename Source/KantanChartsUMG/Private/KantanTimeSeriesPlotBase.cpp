// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

#pragma once

#include "KantanChartsUMG.h"
#include "KantanTimeSeriesPlotBase.h"

#define LOCTEXT_NAMESPACE "KantanCharts"


UKantanTimeSeriesPlotBase::UKantanTimeSeriesPlotBase(FObjectInitializer const& OI) : Super(OI)
{
	DisplayTimeRange = 10.0f;
	bRoundTimeRange = false;
	LowerValueBound.SetFitToData();
	UpperValueBound.SetFitToData();
}

void UKantanTimeSeriesPlotBase::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	auto TimeSeriesPlot = GetTimeSeriesPlot();

	TimeSeriesPlot->SetDisplayTimeRange(DisplayTimeRange);
	TimeSeriesPlot->SetRoundTimeRange(bRoundTimeRange);
	TimeSeriesPlot->SetLowerValueBound(LowerValueBound);
	TimeSeriesPlot->SetUpperValueBound(UpperValueBound);
}

TSharedRef< SWidget > UKantanTimeSeriesPlotBase::RebuildWidget()
{
	MyChart = SNew(SKantanTimeSeriesPlot)
		.Style(&WidgetStyle);
	return MyChart.ToSharedRef();
}


#if WITH_EDITOR

UObject* UKantanTimeSeriesPlotBase::InternalGetPreviewDatasource() const
{
	return FindObject< UObject >(
		::GetTransientPackage(),
		TEXT("PreviewTimeSeriesPlotDatasource")
		);
}

void UKantanTimeSeriesPlotBase::InitPreview()
{
	auto TimeSeriesPlot = GetTimeSeriesPlot();
	auto NumSeries = TimeSeriesPlot->GetNumSeries();
	for (int32 Idx = 0; Idx < NumSeries; ++Idx)
	{
		auto Id = TimeSeriesPlot->GetSeriesId(Idx);
		TimeSeriesPlot->EnableSeries(Id, true);
		TimeSeriesPlot->ConfigureSeries(Id, false, true);
	}
}

#endif


#undef LOCTEXT_NAMESPACE


