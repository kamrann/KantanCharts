// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "Style/KantanCategoryStyle.h"
#include "Engine/DataAsset.h"
#include "KantanCategoryStyleSet.generated.h"


// An asset storing a list of category styles used with a KantanCategoryChart
UCLASS()
class KANTANCHARTSSLATE_API UKantanCategoryStyleSet: public UDataAsset
{
	GENERATED_BODY()

public:
	/*
	List of category styles in this style set.
	*/
	UPROPERTY(EditAnywhere, Category = "Style")
	TArray < FKantanCategoryStyle > Styles;
};


