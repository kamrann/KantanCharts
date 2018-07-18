// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "FloatRoundingLevel.h"
#include "Math/TransformCalculus2D.h"


struct FCartesianAxisRange;

namespace AxisUtil {

	/*
	Represents a 1D transform mapping an axis between 2 spaces, e.g. from value space to chart local space.
	*/
	struct FAxisTransform
	{
		FTransform2D Xform;
		int32 AxisIdx;

		static inline FAxisTransform FromTransform2D(const FTransform2D& InXform, int32 InAxisIdx)
		{
			return FAxisTransform{ InXform, InAxisIdx };
		}

		inline float MapPoint(float InValue) const
		{
			FVector2D Pnt = FVector2D::ZeroVector;
			Pnt[AxisIdx] = InValue;
			return Xform.TransformPoint(Pnt)[AxisIdx];
		}

		inline float MapLength(float InValue) const
		{
			FVector2D Vec = FVector2D::ZeroVector;
			Vec[AxisIdx] = InValue;
			return Xform.TransformVector(Vec)[AxisIdx];
		}

		inline FAxisTransform Inverse() const
		{
			return FAxisTransform{ ::Inverse(Xform), AxisIdx };
		}

		// Tests to make sure there are no NAN values in the transform
		inline bool CheckValidity() const
		{
			return !FMath::IsNaN(Xform.GetMatrix().Determinant()) && !Xform.GetTranslation().ContainsNaN();
		}
	};

	/*
	Stores information about markers along a section of an axis.
	*/
	struct FAxisMarkerData
	{
		FFloatRoundingLevel					RL;					// Rounding level used for axis markers
		TArray< FFloatRoundedValue >		MarkerValues;		// The values to be marked on the axis
		// Values are treated as: [Offset +] MarkerValue x10^DisplayPower
		int32								DisplayPower;
		TOptional< FFloatRoundedValue >		Offset;

		FAxisMarkerData() :
			RL(),
			MarkerValues(),
			DisplayPower(0),
			Offset()
		{}
	};

	FFloatRoundingLevel DetermineAxisRoundingLevel(
		FAxisTransform const& ValueToChartAxisTransform,	// Transform from value space to chart space along axis in question
		float MinChartSpaceMarkerSeparation					// Minimum required separation between axis markers, in chart space
		);

	FAxisMarkerData DetermineAxisMarkerData(
		FFloatRoundingLevel const& RLevel,			// Rounding level to use for the axis markers
		FCartesianAxisRange const& AxisRange,		// Range of the axis, in value space
		int32 MaxLabelDigits						// Maximum number of digits to display on a marker label
		);

}


