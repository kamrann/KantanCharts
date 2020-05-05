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
	/**
	 * Creates a new datasource object, for use with Advanced versions of Cartesian Plot and Time Series Plot.
	 * @param MaxDatapoints An optional cap on the number of datapoints. New points will replace old ones when the limit is reached. A value of 0 implies no limit.
	 */
	UFUNCTION(BlueprintCallable, Category = "Kantan Charts")
	static UKantanSimpleCartesianDatasource* NewSimpleCartesianDatasource(int32 MaxDatapoints = 0);

public:
	UKantanSimpleCartesianDatasource();

public:
	/**
	 * Adds a new series, using the given ID. If the ID is already in use, the function will fail.
	 */
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Add Series With Id"))
	void BP_AddSeriesWithId(FName Id, FText Name, bool& bSuccess);

	/**
	 * Adds a new series using an auto-generated ID, which is returned.
	 */
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Add Series"))
	void BP_AddSeries(FText Name, FName& SeriesId);

	/**
	 * Removes the series with the given ID.
	 */
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Remove Series"))
	void BP_RemoveSeries(FName Id, bool& bSuccess);

	/**
	 * Removes all series.
	 */
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Remove All Series"))
	void BP_RemoveAllSeries();

	/**
	 * Sets a limit for max the number of allowed datapoints in any series.
	 * A value of 0 implies no maximum.
	 * With a limit set, adding points when at the limit will cause previously added points to be dropped,
	 * in a first in first out manner.
	 */
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Set Datapoint Limit"))
	void BP_SetDatapointLimit(int32 Limit = 0);

	/**
	 * Adds a datapoint to the given series.
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


