// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "KantanSimpleCartesianDatasource.h"
#include "UObject/Package.h"


// @TODO: Should really be in separate cpp file
const FString FKantanSeriesDataList::SeriesIdPrefix = TEXT("KantanSimpleSeries");


UKantanSimpleCartesianDatasource* UKantanSimpleCartesianDatasource::NewSimpleCartesianDatasource(int32 MaxDatapoints)
{
	auto const Datasource = NewObject< UKantanSimpleCartesianDatasource >(GetTransientPackage());
	Datasource->SetDatapointLimit(FMath::Max(MaxDatapoints, 0));
	return Datasource;
}


UKantanSimpleCartesianDatasource::UKantanSimpleCartesianDatasource()
{
	
}

void UKantanSimpleCartesianDatasource::BP_AddSeriesWithId(FName Id, FText Name, bool& bSuccess)
{
	bSuccess = AddSeries(Id, Name);
}

void UKantanSimpleCartesianDatasource::BP_AddSeries(FText Name, FName& SeriesId)
{
	SeriesId = AddSeries(Name);
}

void UKantanSimpleCartesianDatasource::BP_RemoveSeries(FName Id, bool& bSuccess)
{
	bSuccess = RemoveSeries(Id);
}

void UKantanSimpleCartesianDatasource::BP_RemoveAllSeries()
{
	RemoveAllSeries();
}

void UKantanSimpleCartesianDatasource::BP_SetDatapointLimit(int32 Limit)
{
    SetDatapointLimit(FMath::Max(Limit, 0));
}

void UKantanSimpleCartesianDatasource::BP_AddDatapoint(FName SeriesId, FVector2D const& Point, bool& bSuccess)
{
	bSuccess = AddDatapoint(SeriesId, Point);
}


int32 UKantanSimpleCartesianDatasource::GetNumSeries_Implementation() const
{
	return Elements.Num();
}

FName UKantanSimpleCartesianDatasource::GetSeriesId_Implementation(int32 SeriesIdx) const
{
	return Elements[SeriesIdx].Id;
}

FText UKantanSimpleCartesianDatasource::GetSeriesName_Implementation(int32 SeriesIdx) const
{
	return Elements[SeriesIdx].Name;
}

TArray< FKantanCartesianDatapoint > UKantanSimpleCartesianDatasource::GetSeriesDatapoints_Implementation(int32 SeriesIdx) const
{
	return Elements[SeriesIdx].Points;
}


