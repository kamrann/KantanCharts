// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

#pragma once

#include "KantanChartStyle.h"
#include "KantanBarChartStyle.generated.h"


USTRUCT(BlueprintType)
struct KANTANCHARTSSLATE_API FKantanBarChartStyle : public FKantanChartStyle
{
	GENERATED_BODY()

	/*
	Bar opacity: 1.0 = Fully opaque, 0.0 = Fully transparent.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BarOpacity;

	/*
	Outline opacity.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BarOutlineOpacity;

	FKantanBarChartStyle() :
		FKantanChartStyle()
		, BarOpacity(1.0f)
		, BarOutlineOpacity(1.0f)
	{}

	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };

	static const FKantanBarChartStyle& GetDefault();
};


UCLASS(BlueprintType, HideCategories = Object, MinimalAPI)
class UKantanBarChartWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FKantanBarChartStyle ChartStyle;

public:
	virtual const FSlateWidgetStyle* const GetStyle() const override
	{
		return &ChartStyle;
	}
};


