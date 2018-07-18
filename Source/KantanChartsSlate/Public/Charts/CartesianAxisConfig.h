// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "UObject/ObjectMacros.h"
#include "CartesianAxisConfig.generated.h"


UENUM(BlueprintType)
enum class EChartAxisPosition: uint8
{
	LeftBottom	UMETA(DisplayName = "Left/Bottom"),
	RightTop	UMETA(DisplayName = "Right/Top"),
	Floating	UMETA(DisplayName = "Floating"),
};

USTRUCT(BlueprintType)
struct FCartesianAxisInstanceConfig
{
	GENERATED_BODY()

	/*
	Is the axis shown?
	*/
	UPROPERTY(EditAnywhere, Category = "Axis")
	bool bEnabled;

	UPROPERTY(EditAnywhere, Category = "Axis")
	bool bShowTitle;

	UPROPERTY(EditAnywhere, Category = "Axis")
	bool bShowMarkers;

	UPROPERTY(EditAnywhere, Category = "Axis")
	bool bShowLabels;

	FCartesianAxisInstanceConfig() :
		bEnabled(false)
		, bShowTitle(false)
		, bShowMarkers(true)
		, bShowLabels(true)
	{}
};

USTRUCT(BlueprintType)
struct FCartesianAxisConfig
{
	GENERATED_BODY()

	/*
	Axis title.
	*/
	UPROPERTY(EditAnywhere, Category = "Axis")
	FText Title;

	/*
	Optional unit string for the axis.
	*/
	UPROPERTY(EditAnywhere, Category = "Axis")
	FText Unit;

	/*
	Controls the density of markers along the axis.
	*/
	UPROPERTY(EditAnywhere, Category = "Axis", Meta = (ClampMin = "0.1", ClampMax = "10"))
	float MarkerSpacing;

	/*
	Specifies the maximum number of digits which can be displayed on an axis marker.
	*/
	UPROPERTY(EditAnywhere, Category = "Axis", Meta = (ClampMin = "1", ClampMax = "10"))
	int32 MaxValueDigits;

	UPROPERTY(EditAnywhere, Category = "Axis", Meta = (DisplayName = "Left/Bottom Axis"))
	FCartesianAxisInstanceConfig LeftBottomAxis;

	UPROPERTY(EditAnywhere, Category = "Axis", Meta = (DisplayName = "Right/Top Axis"))
	FCartesianAxisInstanceConfig RightTopAxis;

	UPROPERTY(EditAnywhere, Category = "Axis")
	FCartesianAxisInstanceConfig FloatingAxis;

	FCartesianAxisConfig() :
		Title()
		, Unit()
		, MarkerSpacing(1.0f)
		, MaxValueDigits(5)
		, LeftBottomAxis()
		, RightTopAxis()
		, FloatingAxis()
	{}
};


