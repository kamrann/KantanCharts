// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "KantanChartLegend.h"
#include "KantanCartesianChartBase.h"

#define LOCTEXT_NAMESPACE "KantanCharts"


UKantanChartLegend::UKantanChartLegend(FObjectInitializer const& OI) : Super(OI)
{
	Margins = FMargin(0.0f);
	SeriesPadding = FMargin(0.0f);
//	bShowDataStyle = true;
	Background.TintColor = FLinearColor{ 1.0f, 1.0f, 1.0f, 0.0f };
	FontSize = 0;
	Chart = nullptr;
}

void UKantanChartLegend::SetMargins(FMargin const& InMargins)
{
	Margins = InMargins;
	if (MyWidget.IsValid())
	{
		MyWidget->SetMargins(Margins);
	}
}

void UKantanChartLegend::SetSeriesPadding(FMargin const& InPadding)
{
	SeriesPadding = InPadding;
	if(MyWidget.IsValid())
	{
		MyWidget->SetSeriesPadding(SeriesPadding);
	}
}
/*
void UKantanChartLegend::ShowDataStyle(bool bShow)
{
	bShowDataStyle = bShow;
	if (MyWidget.IsValid())
	{
		MyWidget->SetShowDataStyle(bShowDataStyle);
	}
}
*/
void UKantanChartLegend::SetBackground(FSlateBrush const& InBrush)
{
	Background = InBrush;
	if(MyWidget.IsValid())
	{
		MyWidget->SetBackgroundOverride(&Background);
	}
}

void UKantanChartLegend::SetFontSize(int32 InFontSize)
{
	FontSize = InFontSize;
	if(MyWidget.IsValid())
	{
		MyWidget->SetFontSizeOverride(FontSize);
	}
}

void UKantanChartLegend::SetChart(UKantanCartesianChartBase* InChart)
{
	Chart = InChart;
	if (MyWidget.IsValid())
	{
		TSharedPtr< SKantanCartesianChart > ChartWidget = Chart.IsValid() ? Chart->GetCartesianChart() : nullptr;
		MyWidget->SetChart(ChartWidget);
	}
}

void UKantanChartLegend::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	MyWidget->SetMargins(Margins);
	MyWidget->SetSeriesPadding(SeriesPadding);
//	MyWidget->SetShowDataStyle(bShowDataStyle);
	MyWidget->SetBackgroundOverride(&Background);
	MyWidget->SetFontSizeOverride(FontSize);

	TSharedPtr< SKantanCartesianChart > ChartWidget = Chart.IsValid() ? Chart->GetCartesianChart() : nullptr;
	MyWidget->SetChart(ChartWidget);
}

void UKantanChartLegend::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyWidget.Reset();
}

TSharedRef< SWidget > UKantanChartLegend::RebuildWidget()
{
	MyWidget = SNew(SKantanChartLegend)
		.BackgroundOverride(&Background);
	return MyWidget.ToSharedRef();
}


#if WITH_EDITOR

#include "UMGStyle.h"

const FText UKantanChartLegend::GetPaletteCategory()
{
	return LOCTEXT("KantanChartsWidgetPaletteCategory", "Kantan Charts");
}

#endif


#undef LOCTEXT_NAMESPACE


