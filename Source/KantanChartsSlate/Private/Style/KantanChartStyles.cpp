// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "Style/KantanBarChartStyle.h"
#include "Style/KantanCartesianChartStyle.h"


FKantanBarChartStyle const& FKantanBarChartStyle::GetDefault()
{
	static TUniquePtr< FKantanBarChartStyle > Default;

	if (!Default.IsValid())
	{
		Default.Reset(new FKantanBarChartStyle());
		// @TODO: This should be specified through base class FKantanChartStyle
		// Perhaps just make the constructors assign these defaults, would be simpler
		Default->TitleFontSize = 24;
		Default->Background.TintColor = FLinearColor::Black.CopyWithNewOpacity(0.3f);
		Default->ChartLineColor = FLinearColor::White.CopyWithNewOpacity(0.6f);
	}

	return *Default;
};


FKantanCartesianChartStyle const& FKantanCartesianChartStyle::GetDefault()
{
	static TUniquePtr< FKantanCartesianChartStyle > Default;

	if (!Default.IsValid())
	{
		Default.Reset(new FKantanCartesianChartStyle());
		// @TODO: This should be specified through base class FKantanChartStyle
		// Perhaps just make the constructors assign these defaults, would be simpler
		Default->TitleFontSize = 24;
		Default->Background.TintColor = FLinearColor::Black.CopyWithNewOpacity(0.3f);
		Default->ChartLineColor = FLinearColor::White.CopyWithNewOpacity(0.6f);
	}

	return *Default;
};
