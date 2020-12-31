// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "IDataSeriesElement.h"
#include "KantanCartesianDatapoint.h"

#include "Layout/Geometry.h"
#include "Rendering/DrawElements.h"
#include "Brushes/SlateImageBrush.h"
#include "Engine/Texture2D.h"


class FSlateBoxSeriesElement : public IDataSeriesElement
{
public:
	FSlateBoxSeriesElement(UTexture2D* Tex, FVector2D PntSz, FLinearColor Cl, FBox2D UVs);

	virtual void RenderSeries(
		const FGeometry& PlotSpaceGeometry,
		const FSlateRect& ClipRect,
		const FSlateRenderTransform& CartesianToPlotXform,
		TArray< FVector2D >&& Points,
		int32 LayerId,
		FSlateWindowElementList& OutDrawElements,
		TArray <FKantanDrawColor> && DrawColors
	) override;

public:
	FVector2D PointSize;
	FLinearColor Color;
	FSlateImageBrush PointBrush;
};


FSlateBoxSeriesElement::FSlateBoxSeriesElement(UTexture2D* Tex, FVector2D PntSz, FLinearColor Cl, FBox2D UVs): PointSize(PntSz), Color(Cl), PointBrush(Tex, PointSize, FLinearColor::White)
{
	PointBrush.SetUVRegion(UVs);
}

void FSlateBoxSeriesElement::RenderSeries(
	const FGeometry& PlotSpaceGeometry,
	const FSlateRect& ClipRect,
	const FSlateRenderTransform& CartesianToPlotXform,
	TArray< FVector2D >&& Points,
	int32 LayerId,
	FSlateWindowElementList& OutDrawElements,
	TArray <FKantanDrawColor> && DrawColors
)
{
	const auto Transform = CartesianToPlotXform;

	// @NOTE: Removing scale in order to retain old behaviour of drawing points at a fixed size on screen, regardless of widget size and scaling.
	// Not sure this is best, but prefer to keep consistent.
	const auto ExistingScale = PlotSpaceGeometry.GetAccumulatedLayoutTransform().GetScale();
	const auto ScaleAdjustment = (ExistingScale == 0.0f ? 0.0f : (1.0f / ExistingScale));

	auto const Count = Points.Num();
	
	for (int32 Idx = 0; Idx < Count; ++Idx)
	// for (const auto& SrcPnt : Points)
	{
		const auto Pnt = Transform.TransformPoint(Points[Idx]);
		auto Geom = PlotSpaceGeometry.MakeChild(PointSize, FSlateLayoutTransform(Pnt));
		Geom = Geom.MakeChild(Geom.GetLocalSize(), FSlateLayoutTransform(ScaleAdjustment));
		// Offset to draw the box centered at the correct coordinates
		Geom = Geom.MakeChild(Geom.GetLocalSize(), FSlateLayoutTransform(-PointSize * 0.5f));

		FLinearColor PointColor = (DrawColors[Idx].DoOverrideColor ? DrawColors[Idx].Color : Color);

		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
			Geom.ToPaintGeometry(),
			&PointBrush,
			ESlateDrawEffect::None,
			PointColor
		);
	}
}


TSharedRef< IDataSeriesElement, ESPMode::ThreadSafe > MakeSlateBoxSeriesElement(UTexture2D* Tex, FVector2D PntSz, FLinearColor Cl, FBox2D UVs)
{
	return MakeShareable(new FSlateBoxSeriesElement(Tex, PntSz, Cl, UVs));
}


