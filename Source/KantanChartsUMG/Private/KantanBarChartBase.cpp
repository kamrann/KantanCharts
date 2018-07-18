// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "KantanBarChartBase.h"


UKantanBarChartBase::UKantanBarChartBase(FObjectInitializer const& OI) : Super(OI)
{
	SKantanBarChart::FArguments SlateDefaults;
	WidgetStyle = *SlateDefaults._Style;

	Orientation = EKantanBarChartOrientation::Vertical;
	MaxBarValue = 0.0f;
	BarToGapRatio = 3.0f;
	LabelPosition = EKantanBarLabelPosition::Standard;
	ValueExtentsDisplay = EKantanBarValueExtents::ZeroLineOnly;
	ValueAxisCfg.LeftBottomAxis.bEnabled = true;
}

void UKantanBarChartBase::SetOrientation(EKantanBarChartOrientation InOrientation)
{
	Orientation = InOrientation;
	if (MyChart.IsValid())
	{
		GetBarChart()->SetOrientation(Orientation);
	}
}

void UKantanBarChartBase::SetMaxBarValue(float InMaxValue)
{
	MaxBarValue = InMaxValue;
	if (MyChart.IsValid())
	{
		GetBarChart()->SetMaxBarValue(MaxBarValue);
	}
}

void UKantanBarChartBase::SetLabelPosition(EKantanBarLabelPosition InPosition)
{
	LabelPosition = InPosition;
	if (MyChart.IsValid())
	{
		GetBarChart()->SetLabelPosition(LabelPosition);
	}
}

void UKantanBarChartBase::SetBarToGapRatio(float InRatio)
{
	BarToGapRatio = InRatio;
	if (MyChart.IsValid())
	{
		GetBarChart()->SetBarToGapRatio(BarToGapRatio);
	}
}

void UKantanBarChartBase::SetExtentsDisplay(EKantanBarValueExtents InExtents)
{
	ValueExtentsDisplay = InExtents;
	if (MyChart.IsValid())
	{
		GetBarChart()->SetValueExtentsDisplay(ValueExtentsDisplay);
	}
}

void UKantanBarChartBase::SetValueAxisConfig(FCartesianAxisConfig const& InCfg)
{
	ValueAxisCfg = InCfg;
	if (MyChart.IsValid())
	{
		GetBarChart()->SetValueAxisConfig(ValueAxisCfg);
	}
}

void UKantanBarChartBase::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	auto BarChart = GetBarChart();
	
	BarChart->SetOrientation(Orientation);
	BarChart->SetMaxBarValue(MaxBarValue);
	BarChart->SetLabelPosition(LabelPosition);
	BarChart->SetBarToGapRatio(BarToGapRatio);
	BarChart->SetValueExtentsDisplay(ValueExtentsDisplay);
	BarChart->SetValueAxisConfig(ValueAxisCfg);
}

TSharedRef< SWidget > UKantanBarChartBase::RebuildWidget()
{
	MyChart = SNew(SKantanBarChart)
		.Style(&WidgetStyle);
	return MyChart.ToSharedRef();
}


