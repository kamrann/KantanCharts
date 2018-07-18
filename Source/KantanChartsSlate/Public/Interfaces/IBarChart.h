// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "Interfaces/ICategoryChart.h"
#include "KantanCategoryTypes.h"


struct FKantanBarChartStyle;
struct FCartesianAxisConfig;

namespace KantanCharts {

	class IBarChart: public virtual ICategoryChart
	{
	public:
		virtual void SetStyle(const FKantanBarChartStyle* InStyle) = 0;
		virtual void SetStyleFromAsset(USlateWidgetStyleContainerBase* InStyleAsset) = 0;
		virtual void SetOrientation(EKantanBarChartOrientation InOrientation) = 0;
		virtual void SetMaxBarValue(float InMax) = 0;
		virtual void SetLabelPosition(EKantanBarLabelPosition InPosition) = 0;
		virtual void SetBarToGapRatio(float InRatio) = 0;
		virtual void SetDrawCategoryBoundaries(bool InDraw) = 0;
		virtual void SetValueExtentsDisplay(EKantanBarValueExtents InExtents) = 0;
		virtual void SetValueAxisConfig(FCartesianAxisConfig const& InConfig) = 0;
	};

}

