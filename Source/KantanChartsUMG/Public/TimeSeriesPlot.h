// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "KantanTimeSeriesPlotBase.h"
#include "TimeSeriesPlot.generated.h"


/*
A cartesian plot widget specialized for plotting time-varying series.
Can be hooked up to any datasource object implementing IKantanCartesianDatasourceInterface.
*/
UCLASS(Meta = (DisplayName = "Time Series Plot (Advanced)"))
class KANTANCHARTSUMG_API UTimeSeriesPlot : public UKantanTimeSeriesPlotBase
{
	GENERATED_BODY()

public:
	UTimeSeriesPlot(FObjectInitializer const&);

public:
	/*
	Set the data source - the object must implement IKantanCartesianDatasourceInterface for this call to succeed.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (Keywords = "Kantan Charts"))
	bool SetDatasource(UObject* InDatasource);

protected:
	virtual UObject* InternalGetDatasource() override
	{
		return Datasource;
	}

#if WITH_EDITOR
	virtual bool UseDatasourceAtDesignTime() const override
	{
		// If Datasource property is set at design time, then use it
		return true;
	}

public:
	virtual void PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent) override;
#endif

protected:
	/*
	Datasource for the plot. Must implement IKantanCartesianDatasourceInterface.
	Generally you should not set this directly in the editor, but through a call to SetDatasource.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "Config", Meta = (DisplayName = "Datasource"))
	class UObject* Datasource;
	// @TODO: EditAnywhere, - Ideally we have a customization that allows this to be bound to a property on the outer UUserWidget.
	// Could be done with the PropName, PropNameDelegate pattern, but this allows a static value to be set, which doesn't make much sense in this context.
};


