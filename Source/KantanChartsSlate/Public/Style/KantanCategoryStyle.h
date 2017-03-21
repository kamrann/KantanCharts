// Copyright (C) 2015-2017 Cameron Angus. All Rights Reserved.

#pragma once

#include "KantanCategoryStyle.generated.h"


// Defines visual properties that can be configured per category
USTRUCT(BlueprintType)
struct FKantanCategoryStyle
{
	GENERATED_BODY()

	/*
	Unique ID for the style.
	*/
	UPROPERTY(EditAnywhere, Category = "Style")
	FName CategoryStyleId;

	/*
	Color to draw the category.
	*/
	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor Color;

//	UPROPERTY(EditAnywhere, Category = "Style")
//	FSlateBrush Brush;

	FKantanCategoryStyle() :
		CategoryStyleId(NAME_None)
		, Color(FLinearColor::White)
//		, Brush()
	{
//		Brush.DrawAs = ESlateBrushDrawType::NoDrawType;
	}
};


