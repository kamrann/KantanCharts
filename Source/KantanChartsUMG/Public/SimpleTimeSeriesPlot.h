// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "KantanTimeSeriesPlotBase.h"
#include "SimpleCartesianDatasourceImpl.h"
#include "KantanCartesianDatasourceInterface.h"
#include "SimpleTimeSeriesPlot.generated.h"


/*
A simple cartesian plot widget specialized for plotting time-varying series, with integrated datasource.
*/
UCLASS(Meta = (DisplayName = "Time Series Plot"))
class KANTANCHARTSUMG_API USimpleTimeSeriesPlot :
	public UKantanTimeSeriesPlotBase
	, public FKantanSeriesDataList
	, public IKantanCartesianDatasourceInterface
{
	GENERATED_BODY()

public:
	USimpleTimeSeriesPlot(FObjectInitializer const&);

public:
	/*
	Add a series using the specified ID. Will fail if the ID is already in use.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Add Series With Id", Keywords = "Kantan Charts"))
	void BP_AddSeriesWithId(bool& bSuccess, FName Id, FText Name, bool bEnabled = true, bool bShowPoints = true, bool bShowLines = false);

	/*
	Add a series and return an auto-generated ID.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Add Series", Keywords = "Kantan Charts"))
	void BP_AddSeries(FName& SeriesId, FText Name, bool bEnabled = true, bool bShowPoints = true, bool bShowLines = false);

	/*
	Remove existing series with given ID.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Remove Series", Keywords = "Kantan Charts"))
	void BP_RemoveSeries(FName Id, bool& bSuccess);

	/*
	Remove all series from the plot.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Remove All Series", Keywords = "Kantan Charts"))
	void BP_RemoveAllSeries();

	/*
	Add a datapoint to the specified series.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Add Datapoint", Keywords = "Kantan Charts"))
	void BP_AddDatapoint(FName SeriesId, FVector2D const& Point, bool& bSuccess);

	/*
	Add a datapoint using the current game time as the x-axis value.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Add Datapoint Now", Keywords = "Kantan Charts"))
	void BP_AddDatapointNow(FName SeriesId, float Value, bool& bSuccess);

public:
	// IKantanCartesianDatasourceInterface
	virtual int32 GetNumSeries_Implementation() const;
	virtual FName GetSeriesId_Implementation(int32 SeriesIdx) const;
	virtual FText GetSeriesName_Implementation(int32 SeriesIdx) const;
	virtual TArray< FKantanCartesianDatapoint > GetSeriesDatapoints_Implementation(int32 SeriesIdx) const;
	//

protected:
	virtual UObject* InternalGetDatasource() override
	{
		return this;
	}

#if WITH_EDITOR
	virtual bool UseDatasourceAtDesignTime() const override
	{
		// Never use our own datasource at design time, as it is runtime configured only
		return false;
	}
#endif
};


