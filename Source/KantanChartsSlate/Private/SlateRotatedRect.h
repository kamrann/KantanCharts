// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateRect.h"

/*
@TODO:
Copied from engine source, need to remove if/when this is exported by the engine.
*/


struct FSlateRotatedRect2
{
	/** Default ctor. */
	FSlateRotatedRect2();
	/** Construct a rotated rect from a given aligned rect. */
	explicit FSlateRotatedRect2(const FSlateRect& AlignedRect);
	/** Per-element constructor. */
	FSlateRotatedRect2(const FVector2D& InTopLeft, const FVector2D& InExtentX, const FVector2D& InExtentY);

	/** transformed Top-left corner. */
	FVector2D TopLeft;
	/** transformed X extent (right-left). */
	FVector2D ExtentX;
	/** transformed Y extent (bottom-top). */
	FVector2D ExtentY;

	/** Convert to a bounding, aligned rect. */
	FSlateRect ToBoundingRect() const;
	/** Point-in-rect test. */
//	bool IsUnderLocation(const FVector2D& Location) const;
};


inline FSlateRotatedRect2::FSlateRotatedRect2() {}

inline FSlateRotatedRect2::FSlateRotatedRect2(const FSlateRect& AlignedRect)
	: TopLeft(AlignedRect.GetTopLeft())
	, ExtentX(AlignedRect.Right - AlignedRect.Left, 0.0f)
	, ExtentY(0.0f, AlignedRect.Bottom - AlignedRect.Top)
{
}

inline FSlateRotatedRect2::FSlateRotatedRect2(const FVector2D& InTopLeft, const FVector2D& InExtentX, const FVector2D& InExtentY)
	: TopLeft(InTopLeft)
	, ExtentX(InExtentX)
	, ExtentY(InExtentY)
{
}

// !!! WRH 2014/08/25 - this is a brute-force, not efficient implementation, uses a bunch of extra conditionals.
inline FSlateRect FSlateRotatedRect2::ToBoundingRect() const
{
	FVector2D Points[4] =
	{
		TopLeft,
		TopLeft + ExtentX,
		TopLeft + ExtentY,
		TopLeft + ExtentX + ExtentY
	};
	return FSlateRect(
		FMath::Min(Points[0].X, FMath::Min3(Points[1].X, Points[2].X, Points[3].X)),
		FMath::Min(Points[0].Y, FMath::Min3(Points[1].Y, Points[2].Y, Points[3].Y)),
		FMath::Max(Points[0].X, FMath::Max3(Points[1].X, Points[2].X, Points[3].X)),
		FMath::Max(Points[0].Y, FMath::Max3(Points[1].Y, Points[2].Y, Points[3].Y))
		);
}


template <typename TransformType>
inline FSlateRotatedRect2 TransformRect(const TransformType& Transform, const FSlateRotatedRect2& Rect)
{
	return FSlateRotatedRect2
		(
		TransformPoint(Transform, Rect.TopLeft),
		TransformVector(Transform, Rect.ExtentX),
		TransformVector(Transform, Rect.ExtentY)
		);
}


