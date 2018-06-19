// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "SimpleCartesianPlot.h"

#define LOCTEXT_NAMESPACE "KantanCharts"


USimpleCartesianPlot::USimpleCartesianPlot(FObjectInitializer const& OI) : Super(OI)
{
	
}

void USimpleCartesianPlot::BP_AddSeriesWithId(
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

void USimpleCartesianPlot::BP_AddSeries(
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

void USimpleCartesianPlot::BP_RemoveSeries(FName Id, bool& bSuccess)
{
	bSuccess = RemoveSeries(Id);
}

void USimpleCartesianPlot::BP_RemoveAllSeries()
{
	RemoveAllSeries();
}

void USimpleCartesianPlot::BP_AddDatapoint(FName SeriesId, FVector2D const& Point, bool& bSuccess)
{
	bSuccess = AddDatapoint(SeriesId, Point);
}


int32 USimpleCartesianPlot::GetNumSeries_Implementation() const
{
	return Elements.Num();
}

FName USimpleCartesianPlot::GetSeriesId_Implementation(int32 SeriesIdx) const
{
	return Elements[SeriesIdx].Id;
}

FText USimpleCartesianPlot::GetSeriesName_Implementation(int32 SeriesIdx) const
{
	return Elements[SeriesIdx].Name;
}

TArray< FKantanCartesianDatapoint > USimpleCartesianPlot::GetSeriesDatapoints_Implementation(int32 SeriesIdx) const
{
	return Elements[SeriesIdx].Points;
}



#undef LOCTEXT_NAMESPACE


