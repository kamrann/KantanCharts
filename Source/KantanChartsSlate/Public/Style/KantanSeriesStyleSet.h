// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

#pragma once

#include "KantanSeriesStyle.h"
#include "KantanSeriesStyleSet.generated.h"


// An asset storing a list of series styles used with a KantanCartesianChart
UCLASS()
class KANTANCHARTSSLATE_API UKantanSeriesStyleSet: public UObject
{
	GENERATED_BODY()

public:
	/*
	List of series styles in this style set.
	*/
	UPROPERTY(EditAnywhere, Category = "Style")
	TArray < FKantanSeriesStyle > Styles;
};


