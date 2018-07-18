// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


struct FPointerEvent;


namespace KantanCharts
{
	enum class EChartInteractionArea: uint8 {
		Plot,
		XAxis,
		YAxis,
	};

	DECLARE_DELEGATE_TwoParams(FOnInteractionMouseDown, EChartInteractionArea, const FPointerEvent&);
}
