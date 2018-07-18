// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "Style/KantanChartStyle.h"
#include "Styling/SlateWidgetStyleContainerBase.h"
#include "KantanBarChartStyle.generated.h"


USTRUCT(BlueprintType)
struct KANTANCHARTSSLATE_API FKantanBarChartStyle : public FKantanChartStyle
{
	GENERATED_BODY()

	/*
	Bar opacity: 1.0 = Fully opaque, 0.0 = Fully transparent.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	float BarOpacity;

	/*
	Outline opacity.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	float BarOutlineOpacity;

	/*
	Thickness of bar outlines.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	float BarOutlineThickness;

	FKantanBarChartStyle() :
		FKantanChartStyle()
		, BarOpacity(1.0f)
		, BarOutlineOpacity(1.0f)
		, BarOutlineThickness(1.0f)
	{}

	static const FName TypeName;
	virtual const FName GetTypeName() const override;// { return TypeName; };

	static const FKantanBarChartStyle& GetDefault();
};


UCLASS(BlueprintType, HideCategories = Object, MinimalAPI)
class UKantanBarChartWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Style")
	FKantanBarChartStyle ChartStyle;

public:
	virtual const FSlateWidgetStyle* const GetStyle() const override
	{
		return &ChartStyle;
	}
};


