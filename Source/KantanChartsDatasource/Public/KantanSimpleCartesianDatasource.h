// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "SimpleCartesianDatasourceImpl.h"
#include "KantanCartesianDatasourceInterface.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "UObject/TextProperty.h"
#include "KantanSimpleCartesianDatasource.generated.h"


UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class KANTANCHARTSDATASOURCE_API UKantanSimpleCartesianDatasource :
	public UObject
	, public FKantanSeriesDataList
	, public IKantanCartesianDatasourceInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Kantan Charts")
	static UKantanSimpleCartesianDatasource* NewSimpleCartesianDatasource();

public:
	UKantanSimpleCartesianDatasource();

public:
	/*
	Adds a new series, using the given ID. If the ID is already in use, the function will fail.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Add Series With Id"))
	void BP_AddSeriesWithId(FName Id, FText Name, bool& bSuccess);

	/*
	Adds a new series using an auto-generated ID, which is returned.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Add Series"))
	void BP_AddSeries(FText Name, FName& SeriesId);

	/*
	Removes the series with the given ID.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Remove Series"))
	void BP_RemoveSeries(FName Id, bool& bSuccess);

	/*
	Removes all series.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Remove All Series"))
	void BP_RemoveAllSeries();

	/*
	Adds a datapoint to the given series.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Add Datapoint"))
	void BP_AddDatapoint(FName SeriesId, FVector2D const& Point, bool& bSuccess);

public:
	// IKantanCartesianDatasourceInterface
	virtual int32 GetNumSeries_Implementation() const;
	virtual FName GetSeriesId_Implementation(int32 SeriesIdx) const;
	virtual FText GetSeriesName_Implementation(int32 SeriesIdx) const;
	virtual TArray< FKantanCartesianDatapoint > GetSeriesDatapoints_Implementation(int32 SeriesIdx) const;
	//
};


