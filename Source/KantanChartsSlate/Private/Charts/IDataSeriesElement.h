// Copyright (C) 2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "Templates/SharedPointer.h"
#include "Rendering/SlateRenderTransform.h"


struct FGeometry;
class FSlateRect;
class FSlateWindowElementList;

class IDataSeriesElement : public TSharedFromThis< IDataSeriesElement, ESPMode::ThreadSafe >
{
public:
	virtual void RenderSeries(
		const FGeometry& PlotSpaceGeometry,
		const FSlateRect& ClipRect,
		const FSlateRenderTransform& CartesianToPlotXform,
		TArray< FVector2D >&& Points,
		int32 LayerId,
		FSlateWindowElementList& OutDrawElements
	) = 0;

	virtual ~IDataSeriesElement() = default;
};

