// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "KantanCartesianTypes.h"
#include "Engine/DataAsset.h"
#include "KantanSeriesStyle.generated.h"


class UTexture2D;

UCLASS()
class KANTANCHARTSSLATE_API UKantanPointStyle : public UDataAsset
{
	GENERATED_BODY()

public:
	/*
	Texture providing datapoint image in different sizes.
	*/
	UPROPERTY(EditAnywhere, Category = "Style")
	UTexture2D* DataPointTexture;

	/*
	Pixel (x, y) offsets into the texture for each point size.
	The image sections should be 5x5, 7x7 and 9x9 respectively.
	*/
	UPROPERTY(EditAnywhere, Category = "Style")
	FIntPoint PointSizeTextureOffsets[KantanDataPointSizeCount];

public:
	UKantanPointStyle() :
		DataPointTexture(nullptr)
		, PointSizeTextureOffsets()
	{}

	inline bool HasValidTexture() const
	{
		return DataPointTexture != nullptr;
	}
};

// Defines visual properties that can be configured per series
USTRUCT(BlueprintType)
struct FKantanSeriesStyle
{
	GENERATED_BODY()

	/*
	Unique ID for the series.
	*/
	UPROPERTY(EditAnywhere, Category = "Style")
	FName StyleId;

	/*
	Point style asset.
	*/
	UPROPERTY(EditAnywhere, Category = "Style")
	UKantanPointStyle* PointStyle;

	/*
	Color to use when drawing the series.
	*/
	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor Color;

	FKantanSeriesStyle() :
		StyleId(NAME_None)
		, PointStyle(nullptr)
		, Color(FLinearColor::White)
	{}

	inline bool HasValidPointStyle() const
	{
		return PointStyle != nullptr && PointStyle->HasValidTexture();
	}
};


