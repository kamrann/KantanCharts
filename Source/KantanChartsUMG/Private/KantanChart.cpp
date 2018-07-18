// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "KantanChart.h"

#define LOCTEXT_NAMESPACE "KantanCharts"


UKantanChart::UKantanChart(FObjectInitializer const& OI) : Super(OI)
{
	Margins = FMargin(0.0f);
	ChartTitle = FText();
	TitlePadding = FMargin(0.0f, 5.0f);
	UpdateTickRate = 0.0f;

#if WITH_EDITORONLY_DATA
	bShowDataPreview = true;
#endif
}

void UKantanChart::SetMargins(FMargin const& InMargins)
{
	Margins = InMargins;
	if (MyChart.IsValid())
	{
		MyChart->SetMargins(Margins);
	}
}

void UKantanChart::SetChartTitle(FText const& InTitle)
{
	ChartTitle = InTitle;
	if (MyChart.IsValid())
	{
		MyChart->SetChartTitle(ChartTitle);
	}
}

void UKantanChart::SetChartTitlePadding(FMargin const& InPadding)
{
	TitlePadding = InPadding;
	if (MyChart.IsValid())
	{
		MyChart->SetChartTitlePadding(TitlePadding);
	}
}

void UKantanChart::SetUpdateTickRate(float InRate)
{
	UpdateTickRate = InRate;
	if (MyChart.IsValid())
	{
		MyChart->SetUpdateTickRate(UpdateTickRate);
	}
}

void UKantanChart::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	MyChart->SetMargins(Margins);
	MyChart->SetChartTitle(ChartTitle);
	MyChart->SetChartTitlePadding(TitlePadding);
	MyChart->SetUpdateTickRate(UpdateTickRate);
}

void UKantanChart::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyChart.Reset();
}


#if WITH_EDITOR

#include "UMGStyle.h"
/*
const FSlateBrush* UKantanChart::GetEditorIcon()
{
	return FUMGStyle::Get().GetBrush("Widget.Image");
}
*/
const FText UKantanChart::GetPaletteCategory()
{
	return LOCTEXT("KantanChartsWidgetPaletteCategory", "Kantan Charts");
}

#endif


#undef LOCTEXT_NAMESPACE


