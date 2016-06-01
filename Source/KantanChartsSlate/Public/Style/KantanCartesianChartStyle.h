// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

#pragma once

#include "KantanChartStyle.h"
#include "KantanCartesianChartStyle.generated.h"


USTRUCT(BlueprintType)
struct KANTANCHARTSSLATE_API FKantanCartesianChartStyle : public FKantanChartStyle
{
	GENERATED_BODY()

	/*
	Opacity for rendering data points and lines.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DataOpacity;

	FKantanCartesianChartStyle() :
		FKantanChartStyle()
		, DataOpacity(1.0f)
	{}

	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };

	static const FKantanCartesianChartStyle& GetDefault();
};


UCLASS(BlueprintType, HideCategories = Object, MinimalAPI)
class UKantanCartesianChartWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FKantanCartesianChartStyle ChartStyle;

public:
	virtual const FSlateWidgetStyle* const GetStyle() const override
	{
		return &ChartStyle;
	}
};

