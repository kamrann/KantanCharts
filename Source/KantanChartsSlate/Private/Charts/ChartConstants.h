// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "Layout/Margin.h"
#include "Layout/SlateRect.h"


namespace ChartConstants
{
	// General
	float const AxisMarkerLength = 5.0f;
	float const AxisMarkerLabelGap = 3.0f;

	// @NOTE: Workaround to avoid clipping lines along extreme edges of the chart.
	// Look into why this is needed.
	FMargin const ChartClipRectExtension = FMargin{ 0.0f, 0.0f, 1.0f, 1.0f };

	// Bar Chart
	float const AxisCategoryLabelGap = 0.0f;

	// @NOTE: Slate clipping appears to be unable to adjust to rotated text. Various AnswerHub posts
	// plus PM exchange, but no resolution.
	FSlateRect const RotatedTextClipRect = FSlateRect{ 0.0f, 0.0f, 10000.0f, 10000.0f };
}


