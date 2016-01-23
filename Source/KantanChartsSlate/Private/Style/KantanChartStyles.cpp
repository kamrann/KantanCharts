// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

#include "KantanChartsSlate.h"
#include "KantanBarChartStyle.h"
#include "KantanCartesianChartStyle.h"


const FName FKantanBarChartStyle::TypeName(TEXT("FKantanBarChartStyle"));

const FKantanBarChartStyle& FKantanBarChartStyle::GetDefault()
{
	static TUniquePtr< FKantanBarChartStyle > Default;

	if (!Default.IsValid())
	{
		Default.Reset(new FKantanBarChartStyle());
		// @TODO: This should be specified through base class FKantanChartStyle
		// Perhaps just make the constructors assign these defaults, would be simpler
		Default->Background.TintColor = FLinearColor::Black.CopyWithNewOpacity(0.3f);
		Default->ChartLineColor = FLinearColor::White.CopyWithNewOpacity(0.6f);
	}

	return *Default;
};


const FName FKantanCartesianChartStyle::TypeName(TEXT("FKantanCartesianChartStyle"));

const FKantanCartesianChartStyle& FKantanCartesianChartStyle::GetDefault()
{
	static TUniquePtr< FKantanCartesianChartStyle > Default;

	if (!Default.IsValid())
	{
		Default.Reset(new FKantanCartesianChartStyle());
		// @TODO: This should be specified through base class FKantanChartStyle
		// Perhaps just make the constructors assign these defaults, would be simpler
		Default->Background.TintColor = FLinearColor::Black.CopyWithNewOpacity(0.3f);
		Default->ChartLineColor = FLinearColor::White.CopyWithNewOpacity(0.6f);
	}

	return *Default;
};


