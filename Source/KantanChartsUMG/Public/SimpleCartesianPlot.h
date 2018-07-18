// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "KantanCartesianPlotBase.h"
#include "SimpleCartesianDatasourceImpl.h"
#include "KantanCartesianDatasourceInterface.h"
#include "SimpleCartesianPlot.generated.h"


/*
A simple cartesian plot widget with integrated data source.
*/
UCLASS(Meta = (DisplayName = "Cartesian Plot"))
class KANTANCHARTSUMG_API USimpleCartesianPlot :
	public UKantanCartesianPlotBase
	, public FKantanSeriesDataList
	, public IKantanCartesianDatasourceInterface
{
	GENERATED_BODY()

public:
	USimpleCartesianPlot(FObjectInitializer const&);

public:
	/*
	Adds a new series to the plot, using the given ID. If the ID is already in use, the function will fail.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Add Series With Id", Keywords = "Kantan Charts"))
	void BP_AddSeriesWithId(bool& bSuccess, FName Id, FText Name, bool bEnabled = true, bool bShowPoints = true, bool bShowLines = false);

	/*
	Adds a new series to the plot using an auto-generated ID, which is returned.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Add Series", Keywords = "Kantan Charts"))
	void BP_AddSeries(FName& SeriesId, FText Name, bool bEnabled = true, bool bShowPoints = true, bool bShowLines = false);

	/*
	Removes the series with the given ID.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Remove Series", Keywords = "Kantan Charts"))
	void BP_RemoveSeries(FName Id, bool& bSuccess);

	/*
	Removes all series.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Remove All Series", Keywords = "Kantan Charts"))
	void BP_RemoveAllSeries();

	/*
	Adds a datapoint to the given series.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Add Datapoint", Keywords = "Kantan Charts"))
	void BP_AddDatapoint(FName SeriesId, FVector2D const& Point, bool& bSuccess);

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


