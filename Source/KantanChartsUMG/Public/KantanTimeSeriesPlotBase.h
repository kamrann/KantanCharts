// Copyright (C) 2015-2017 Cameron Angus. All Rights Reserved.

#pragma once

#include "KantanCartesianChartBase.h"
#include "SKantanTimeSeriesPlot.h"
#include "KantanTimeSeriesPlotBase.generated.h"


/*
UMG class implementing a cartesian chart specialized for plotting time-varying series.
*/
UCLASS(Abstract)
class KANTANCHARTSUMG_API UKantanTimeSeriesPlotBase : public UKantanCartesianChartBase
{
	GENERATED_BODY()

public:
	/*
	The length of time to display data for.
	If 0, the entire time range of the data is displayed.
	*/
	UPROPERTY(EditAnywhere, Category = "Config")
	float DisplayTimeRange;

	/*
	Whether or not to round off the displayed time range.
	*/
	UPROPERTY(EditAnywhere, Category = "Config")
	bool bRoundTimeRange;

	/*
	Lower bound for the Y (value) axis.
	*/
	UPROPERTY(EditAnywhere, Category = "Config")
	FCartesianRangeBound LowerValueBound;

	/*
	Upper bound for the Y (value) axis.
	*/
	UPROPERTY(EditAnywhere, Category = "Config")
	FCartesianRangeBound UpperValueBound;

public:
	UKantanTimeSeriesPlotBase(FObjectInitializer const&);

public:

public:
	// UWidget interface
	virtual void SynchronizeProperties() override;
	// End of UWidget interface

protected:
	// UWidget interface
	virtual TSharedRef< SWidget > RebuildWidget() override;
	// End of UWidget interface

	inline TSharedPtr< SKantanTimeSeriesPlot > GetTimeSeriesPlot()
	{
		return StaticCastSharedPtr< SKantanTimeSeriesPlot >(MyChart);
	}

#if WITH_EDITOR
protected:
	virtual UObject* InternalGetPreviewDatasource() const override;
	virtual void InitPreview() override;
#endif
};


