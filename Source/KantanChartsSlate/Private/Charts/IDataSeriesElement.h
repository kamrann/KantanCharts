// Copyright (C) 2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "SharedPointer.h"
#include "SlateRenderTransform.h"


struct FGeometry;
class FSlateRect;
class FSlateWindowElementList;

class IDataSeriesElement : public TSharedFromThis< IDataSeriesElement, ESPMode::ThreadSafe >
{
public:
	virtual FSlateRenderTransform GetPointTransform(const FSlateRenderTransform& CartesianToPlotXform, const FGeometry& PlotSpaceGeometry) const = 0;
	virtual void RenderSeries(
		const FGeometry& PlotSpaceGeometry,
		const FSlateRect& ClipRect,
		TArray< FVector2D >&& Points,
		int32 LayerId,
		FSlateWindowElementList& OutDrawElements
	) = 0;

	virtual ~IDataSeriesElement() = default;
};

