// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "SimpleTimeSeriesPlot.h"

#include "Blueprint/WidgetTree.h"

#define LOCTEXT_NAMESPACE "KantanCharts"


USimpleTimeSeriesPlot::USimpleTimeSeriesPlot(FObjectInitializer const& OI) : Super(OI)
{
	
}

void USimpleTimeSeriesPlot::BP_AddSeriesWithId(
	bool& bSuccess,
	FName Id,
	FText Name,
	bool bEnabled,
	bool bShowPoints,
	bool bShowLines
	)
{
	bSuccess = AddSeries(Id, Name);
	if (bSuccess)
	{
		EnableSeries(Id, bEnabled);
		ConfigureSeries(Id, bShowPoints, bShowLines);
	}
}

void USimpleTimeSeriesPlot::BP_AddSeries(
	FName& SeriesId,
	FText Name,
	bool bEnabled,
	bool bShowPoints,
	bool bShowLines)
{
	SeriesId = AddSeries(Name);
	EnableSeries(SeriesId, bEnabled);
	ConfigureSeries(SeriesId, bShowPoints, bShowLines);
}

void USimpleTimeSeriesPlot::BP_RemoveSeries(FName Id, bool& bSuccess)
{
	bSuccess = RemoveSeries(Id);
}

void USimpleTimeSeriesPlot::BP_RemoveAllSeries()
{
	RemoveAllSeries();
}

void USimpleTimeSeriesPlot::BP_AddDatapoint(FName SeriesId, FVector2D const& Point, bool& bSuccess)
{
	bSuccess = AddDatapoint(SeriesId, Point);
}

void USimpleTimeSeriesPlot::BP_AddDatapointNow(FName SeriesId, float Value, bool& bSuccess)
{
	// Our outer is the UWidgetTree, and that has access to the UWorld (via UUserWidget, which is its outer)
	auto const World = Cast< UWidgetTree >(GetOuter())->GetWorld();
	auto const Time = World->GetTimeSeconds();
	auto const Point = FVector2D(Time, Value);
	bSuccess = AddDatapoint(SeriesId, Point);
}


int32 USimpleTimeSeriesPlot::GetNumSeries_Implementation() const
{
	return Elements.Num();
}

FName USimpleTimeSeriesPlot::GetSeriesId_Implementation(int32 SeriesIdx) const
{
	return Elements[SeriesIdx].Id;
}

FText USimpleTimeSeriesPlot::GetSeriesName_Implementation(int32 SeriesIdx) const
{
	return Elements[SeriesIdx].Name;
}

TArray< FKantanCartesianDatapoint > USimpleTimeSeriesPlot::GetSeriesDatapoints_Implementation(int32 SeriesIdx) const
{
	return Elements[SeriesIdx].Points;
}



#undef LOCTEXT_NAMESPACE


