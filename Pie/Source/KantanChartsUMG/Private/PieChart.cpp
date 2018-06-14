// Fill out your copyright notice in the Description page of Project Settings.

#include "KantanChartsUMG.h"
#include "PieChart.h"


UPieChart::UPieChart(FObjectInitializer const& OI) : Super(OI)
{
	Datasource = nullptr;
}

bool UPieChart::SetDatasource(UObject* InDatasource)
{
	if (SKantanCategoryChart::IsNullOrValidDatasource(InDatasource) == false)
	{
		return false;
	}

	Datasource = InDatasource;
	if (MyChart.IsValid())
	{
		GetCategoryChart()->SetDatasource(Datasource);
	}
	return true;
}

void UPieChart::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

#if WITH_EDITOR

const FSlateBrush* UPieChart::GetEditorIcon()
{
	return FKantanChartsStyleSet::Get().GetBrush("Widget.KantanPieChart");
}

#endif


