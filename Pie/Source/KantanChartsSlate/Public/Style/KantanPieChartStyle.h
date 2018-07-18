// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Style/KantanChartStyle.h"
#include "KantanPieChartStyle.generated.h"


USTRUCT(BlueprintType)
struct KANTANCHARTSSLATE_API FKantanPieChartStyle : public FKantanChartStyle
{
	GENERATED_BODY()

	/*
	Opacity of the pie segments.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PieOpacity;

	FKantanPieChartStyle() :
		FKantanChartStyle()
		, PieOpacity(1.0f)
	{}

	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };

	static const FKantanPieChartStyle& GetDefault();
};


UCLASS(BlueprintType, HideCategories = Object, MinimalAPI)
class UKantanPieChartWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FKantanPieChartStyle ChartStyle;

public:
	virtual const FSlateWidgetStyle* const GetStyle() const override
	{
		return &ChartStyle;
	}
};


