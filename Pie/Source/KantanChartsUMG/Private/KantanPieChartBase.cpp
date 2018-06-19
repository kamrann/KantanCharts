// Fill out your copyright notice in the Description page of Project Settings.

#include "KantanChartsUMG.h"
#include "KantanPieChartBase.h"


UKantanPieChartBase::UKantanPieChartBase(FObjectInitializer const& OI) : Super(OI)
{
	SKantanPieChart::FArguments SlateDefaults;
	WidgetStyle = *SlateDefaults._Style;

	static ConstructorHelpers::FObjectFinder< UMaterial > DefaultPieMaterial(
		TEXT("Material'/KantanCharts/Materials/DefaultPieMaterial.DefaultPieMaterial'")
		);
	PieMaterial = DefaultPieMaterial.Object;
}

void UKantanPieChartBase::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	auto PieChart = GetPieChart();

	PieChart->SetPieMaterial(PieMaterial);
}

TSharedRef< SWidget > UKantanPieChartBase::RebuildWidget()
{
	MyChart = SNew(SKantanPieChart)
		.Style(&WidgetStyle);
	return MyChart.ToSharedRef();
}




