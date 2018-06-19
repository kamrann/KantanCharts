// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "TimeSeriesPlot.h"

#define LOCTEXT_NAMESPACE "KantanCharts"


UTimeSeriesPlot::UTimeSeriesPlot(FObjectInitializer const& OI) : Super(OI)
{
	Datasource = nullptr;
}

bool UTimeSeriesPlot::SetDatasource(UObject* InDatasource)
{
	if (SKantanCartesianChart::IsNullOrValidDatasource(InDatasource) == false)
	{
		return false;
	}

	Datasource = InDatasource;
	if (MyChart.IsValid())
	{
		GetTimeSeriesPlot()->SetDatasource(Datasource);
	}
	return true;
}


#if WITH_EDITOR

void UTimeSeriesPlot::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	static const FName NAME_Datasource = GET_MEMBER_NAME_CHECKED(UTimeSeriesPlot, Datasource);

	if (PropertyChangedEvent.Property != NULL)
	{
		const FName PropName = PropertyChangedEvent.MemberProperty->GetFName();
		if (PropName == NAME_Datasource)
		{
			if (SKantanTimeSeriesPlot::IsNullOrValidDatasource(Datasource) == false)
			{
				Datasource = nullptr;
			}
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif


#undef LOCTEXT_NAMESPACE


