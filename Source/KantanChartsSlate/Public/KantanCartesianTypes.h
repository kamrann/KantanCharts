// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "UObject/ObjectMacros.h"
#include "CoreMinimal.h"
#include "Math/TransformCalculus2D.h"
#include "KantanCartesianTypes.generated.h"


UENUM()
enum class ECartesianScalingType : uint8
{
	// Scale specified by fixed multiplier between cartesian space and chart space, along with focal coordinates
	FixedScale,

	// Scale specified indirectly via min and max values along each axis
	FixedRange,
};


USTRUCT(BlueprintType)
struct FCartesianAxisRange
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axis")
	float Min;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axis")
	float Max;

	FCartesianAxisRange() :
		Min(-100.0f),
		Max(100.0f)
	{}

	FCartesianAxisRange(float InMin, float InMax) :
		Min(InMin),
		Max(InMax)
	{}

	FCartesianAxisRange(const FFloatInterval& InInterval):
		Min(InInterval.Min),
		Max(InInterval.Max)
	{}

	inline void Set(float InMin, float InMax)
	{
		Min = InMin;
		Max = InMax;
	}

	inline bool IsZero() const
	{
		return Min == Max;
	}

	inline bool IsInfinite() const
	{
		return Min == -FLT_MAX && Max == FLT_MAX;
	}

	inline bool ContainsNaNOrInf() const
	{
		return FMath::IsNaN(Min) || FMath::IsNaN(Max) || !FMath::IsFinite(Min) || !FMath::IsFinite(Max);
	}

	inline bool IsInversed() const
	{
		return Min > Max;
	}

	inline float Size() const
	{
		return Max - Min;
	}

	inline FCartesianAxisRange Normalized() const
	{
		return FCartesianAxisRange(
			FMath::Min(Min, Max),
			FMath::Max(Min, Max)
			);
	}

	inline bool Contains(float InValue) const
	{
		return InValue >= Min && InValue <= Max;
	}

	inline float Midpoint() const
	{
		return (Min + Max) * 0.5f;
	}

	inline void Expand(float InFactor)
	{
		auto Mid = Midpoint();
		Min = Mid + (Min - Mid) * InFactor;
		Max = Mid + (Max - Mid) * InFactor;
	}
};

USTRUCT()
struct FKantanCartesianPlotScale
{
	GENERATED_BODY()
		
	/*
	@TODO: Ideally this struct's members would all be private and it would just have the below setters plus the
	transform getters. Would then also have getters for the config values of any scaling type, regardless of
	which scaling type was currently set. Problem is, to work out the values for a different scaling type,
	this would need LocalSize to be passed in, and we don't have access to that from the struct's
	detail customization.

	Only way to have struct details update so if you switch scaling type it auto calculates the values based on
	current plot scale, would be to update all the properties in the struct whenever the setters are called,
	as well as whenever the local size of the outer cartesian chart changes. So would require integration
	with chart details customization.

	Might be better to separate out into multiple types, with the enum in the chart, but still have the issue that
	chart size is not a persistent value, but only really available in tick/paint, so would need to hack it a bit.

	void SetAsScaleAndFocus(FVector2D const& Scale, FVector2D const& Focus);
	void SetAsRange(FCartesianAxisRange const& RangeX, FCartesianAxisRange const& RangeY);
	*/

	// The scaling type used
	UPROPERTY(EditAnywhere, Category = "Plot")
	ECartesianScalingType Type;

	/* Fixed scale and focus */

	// Fixed scale multiplier between widget space and cartesian space
	UPROPERTY(EditAnywhere, Category = "Plot")
	FVector2D Scale;

	// Coordinates in cartesian space on which plot should be centered
	UPROPERTY(EditAnywhere, Category = "Plot")
	FVector2D FocalCoordinates;

	/**/

	/* Fixed cartesian range for each axis */

	// X-axis range to be plotted
	UPROPERTY(EditAnywhere, Category = "Plot")
	FCartesianAxisRange RangeX;

	// Y-axis range to be plotted
	UPROPERTY(EditAnywhere, Category = "Plot")
	FCartesianAxisRange RangeY;

	/**/

	FKantanCartesianPlotScale() :
		Type(ECartesianScalingType::FixedScale),
		Scale(1.0f, 1.0f),
		FocalCoordinates(0.0f, 0.0f),
		RangeX(),
		RangeY()
	{}

	inline FTransform2D GetTransform_FixedScale(FVector2D const& LocalSize) const
	{
		return Concatenate(
			FTransform2D(-FocalCoordinates),
			FTransform2D(FScale2D(Scale.X, -Scale.Y)),
			FTransform2D(LocalSize * 0.5f)
			);
	}

	inline FTransform2D GetTransform_FixedRange(FVector2D const& LocalSize) const
	{
		return Concatenate(
			FTransform2D(-FVector2D(RangeX.Min, RangeY.Min)),
			FTransform2D(FScale2D(1.0f / (RangeX.Max - RangeX.Min), 1.0f / (RangeY.Max - RangeY.Min))),
			FTransform2D(FScale2D(LocalSize.X, -LocalSize.Y)),
			FTransform2D(FVector2D(0.0f, LocalSize.Y))
			);
	}

	// Return the axis ranges, regardless of the method in which the plot scale is specified
	inline FCartesianAxisRange GetXRange(FVector2D const& LocalSize) const
	{
		switch (Type)
		{
		case ECartesianScalingType::FixedScale:
		{
			auto PlotToCart = GetTransformToCartesianSpace(LocalSize);
			return FCartesianAxisRange(
				PlotToCart.TransformPoint(FVector2D(0.0f, 0.0f)).X,
				PlotToCart.TransformPoint(FVector2D(LocalSize.X, 0.0f)).X
				);
		}
		case ECartesianScalingType::FixedRange:
			return RangeX;

		default:
			check(false);
			return{};
		}
	}

	inline FCartesianAxisRange GetYRange(FVector2D const& LocalSize) const
	{
		switch (Type)
		{
		case ECartesianScalingType::FixedScale:
		{
			auto PlotToCart = GetTransformToCartesianSpace(LocalSize);
			return FCartesianAxisRange(
				PlotToCart.TransformPoint(FVector2D(0.0f, 0.0f)).Y,
				PlotToCart.TransformPoint(FVector2D(0.0f, LocalSize.Y)).Y
				);
		}
		case ECartesianScalingType::FixedRange:
			return RangeY;

		default:
			check(false);
			return{};
		}
	}

	FTransform2D GetTransformFromCartesianSpace(FVector2D const& LocalSize) const
	{
		switch (Type)
		{
		case ECartesianScalingType::FixedScale:
			return GetTransform_FixedScale(LocalSize);
		case ECartesianScalingType::FixedRange:
			return GetTransform_FixedRange(LocalSize);
		default:
			ensure(false);
			return FTransform2D();
		}
	}

	FTransform2D GetTransformToCartesianSpace(FVector2D const& LocalSize) const
	{
		return Inverse(GetTransformFromCartesianSpace(LocalSize));
	}

	void Validate()
	{
		if (Scale.X == 0.0f)
		{
			Scale.X = 1.0f;
		}
		if (Scale.Y == 0.0f)
		{
			Scale.Y = 1.0f;
		}

		if (RangeX.IsZero())
		{
			RangeX.Set(0.0f, 1.0f);
		}

		if (RangeY.IsZero())
		{
			RangeY.Set(0.0f, 1.0f);
		}
	}
};

struct FCartesianDataSnapshot;
DECLARE_DELEGATE_RetVal_TwoParams(FKantanCartesianPlotScale, FOnUpdatePlotScale, const FCartesianDataSnapshot& /* Data snapshot */, const TArray< int32 >& /* Enabled series */);


UENUM(BlueprintType)
enum class ECartesianRangeBoundType : uint8
{
	FixedValue,
	FitToData,
	FitToDataRounded,
};


USTRUCT(BlueprintType)
struct FCartesianRangeBound
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range")
	ECartesianRangeBoundType Type;

	// Fixed value for the bound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range")
	float FixedBoundValue;

	FCartesianRangeBound():
		Type(ECartesianRangeBoundType::FitToData),
		FixedBoundValue(0.0f)
	{}

	inline void SetFixed(float Value)
	{
		Type = ECartesianRangeBoundType::FixedValue;
		FixedBoundValue = Value;
	}

	inline void SetFitToData()
	{
		Type = ECartesianRangeBoundType::FitToData;
	}

	inline void SetFitToDataRounded()
	{
		Type = ECartesianRangeBoundType::FitToDataRounded;
	}
};


UENUM()
namespace EKantanDataPointSize
{
	enum Type
	{
		// Values correspond to pixel size in the texture
		Small,
		Medium,
		Large,
	};
}

static const int32 KantanDataPointSizeCount = 3;

static const int32 KantanDataPointPixelSizes[KantanDataPointSizeCount] = {
	5,
	7,
	9,
};


