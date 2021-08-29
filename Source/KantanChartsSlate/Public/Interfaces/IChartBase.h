// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "Charts/ChartEvents.h"
#include "CoreMinimal.h"


struct FKantanChartStyle;

struct FMargin;
class SWidget;

namespace KantanCharts
{
	class IChartBase
	{
	public:
		virtual TSharedRef< SWidget > AsWidget() = 0;

	public:
		virtual void SetMargins(FMargin const& InMargins) = 0;
		virtual void SetChartTitle(FText const& InTitle) = 0;
		virtual void SetChartTitlePadding(FMargin const& InPadding) = 0;
		virtual void SetUpdateTickRate(float InRate) = 0;
		virtual void SetOnChartMouseDown(KantanCharts::FOnInteractionMouseDown InDelegate) = 0;

		virtual const FKantanChartStyle* GetChartStyle() const = 0;

	public:
		virtual ~IChartBase() = default;
	};
}
