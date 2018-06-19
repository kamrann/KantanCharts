// Copyright (C) 2015-2017 Cameron Angus. All Rights Reserved.

#include "IDataSeriesElement.h"
#include "Geometry.h"
#include "DrawElements.h"
#include "SlateImageBrush.h"
#include "Engine/Texture2D.h"


class FSlateBoxSeriesElement : public IDataSeriesElement
{
public:
	FSlateBoxSeriesElement(UTexture2D* Tex, FVector2D PntSz, FLinearColor Cl, FBox2D UVs);

	virtual FSlateRenderTransform GetPointTransform(const FSlateRenderTransform& CartesianToPlotXform, const FGeometry& PlotSpaceGeometry) const override;
	virtual void RenderSeries(
		const FGeometry& PlotSpaceGeometry,
		const FSlateRect& ClipRect,
		TArray< FVector2D >&& Points,
		int32 LayerId,
		FSlateWindowElementList& OutDrawElements
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

FSlateRenderTransform FSlateBoxSeriesElement::GetPointTransform(const FSlateRenderTransform& CartesianToPlotXform, const FGeometry& PlotSpaceGeometry) const
{
	return CartesianToPlotXform;
}

void FSlateBoxSeriesElement::RenderSeries(
	const FGeometry& PlotSpaceGeometry,
	const FSlateRect& ClipRect,
	TArray< FVector2D >&& Points,
	int32 LayerId,
	FSlateWindowElementList& OutDrawElements
)
{
	
	for (const auto& Pnt : Points)
	{
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
			PlotSpaceGeometry.MakeChild(PointSize, FSlateLayoutTransform(Pnt)).ToPaintGeometry(),
			&PointBrush,
			ESlateDrawEffect::None,
			Color//SeriesStyle.Color * FLinearColor(1, 1, 1, ChartStyle->DataOpacity)
		);
	}
}


TSharedRef< IDataSeriesElement, ESPMode::ThreadSafe > MakeSlateBoxSeriesElement(UTexture2D* Tex, FVector2D PntSz, FLinearColor Cl, FBox2D UVs)
{
	return MakeShareable(new FSlateBoxSeriesElement(Tex, PntSz, Cl, UVs));
}


