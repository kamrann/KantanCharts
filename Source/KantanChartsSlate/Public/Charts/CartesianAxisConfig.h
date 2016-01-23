// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

#pragma once

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
	UPROPERTY(EditAnywhere)
	bool bEnabled;

	UPROPERTY(EditAnywhere)
	bool bShowTitle;

	UPROPERTY(EditAnywhere)
	bool bShowMarkers;

	UPROPERTY(EditAnywhere)
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
	UPROPERTY(EditAnywhere)
	FText Title;

	/*
	Optional unit string for the axis.
	*/
	UPROPERTY(EditAnywhere)
	FText Unit;

	/*
	Controls the density of markers along the axis.
	*/
	UPROPERTY(EditAnywhere, Meta = (ClampMin = "0.1", ClampMax = "10"))
	float MarkerSpacing;

	/*
	Specifies the maximum number of digits which can be displayed on an axis marker.
	*/
	UPROPERTY(EditAnywhere, Meta = (ClampMin = "1", ClampMax = "10"))
	int32 MaxValueDigits;

	UPROPERTY(EditAnywhere, Meta = (DisplayName = "Left/Bottom Axis"))
	FCartesianAxisInstanceConfig LeftBottomAxis;

	UPROPERTY(EditAnywhere, Meta = (DisplayName = "Right/Top Axis"))
	FCartesianAxisInstanceConfig RightTopAxis;

	UPROPERTY(EditAnywhere)
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


