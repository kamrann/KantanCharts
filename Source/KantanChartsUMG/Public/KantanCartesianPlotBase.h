// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "KantanCartesianChartBase.h"
#include "KantanCartesianPlotBase.generated.h"


/*
UMG class implementing a cartesian chart.
*/
UCLASS(Abstract)
class KANTANCHARTSUMG_API UKantanCartesianPlotBase : public UKantanCartesianChartBase
{
	GENERATED_BODY()

public:
	UKantanCartesianPlotBase(FObjectInitializer const&);

#if WITH_EDITOR
protected:
	virtual UObject* InternalGetPreviewDatasource() const override;
	virtual void InitPreview() override;
#endif
};


