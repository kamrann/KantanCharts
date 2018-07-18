// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "Style/KantanSeriesStyle.h"
#include "Engine/DataAsset.h"
#include "KantanSeriesStyleSet.generated.h"


// An asset storing a list of series styles used with a KantanCartesianChart
UCLASS()
class KANTANCHARTSSLATE_API UKantanSeriesStyleSet: public UDataAsset
{
	GENERATED_BODY()

public:
	/*
	List of series styles in this style set.
	*/
	UPROPERTY(EditAnywhere, Category = "Style")
	TArray < FKantanSeriesStyle > Styles;
};


