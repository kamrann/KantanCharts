// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

#pragma once

#include "KantanCategoryStyle.h"
#include "KantanCategoryStyleSet.generated.h"


// An asset storing a list of category styles used with a KantanCategoryChart
UCLASS()
class KANTANCHARTSSLATE_API UKantanCategoryStyleSet: public UObject
{
	GENERATED_BODY()

public:
	/*
	List of category styles in this style set.
	*/
	UPROPERTY(EditAnywhere, Category = "Style")
	TArray < FKantanCategoryStyle > Styles;
};


