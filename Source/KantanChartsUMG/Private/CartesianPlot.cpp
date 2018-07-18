// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "CartesianPlot.h"

#define LOCTEXT_NAMESPACE "KantanCharts"


UCartesianPlot::UCartesianPlot(FObjectInitializer const& OI) : Super(OI)
{
	Datasource = nullptr;
}

bool UCartesianPlot::SetDatasource(UObject* InDatasource)
{
	if (SKantanCartesianChart::IsNullOrValidDatasource(InDatasource) == false)
	{
		return false;
	}

	Datasource = InDatasource;
	if (MyChart.IsValid())
	{
		GetCartesianChart()->SetDatasource(Datasource);
	}
	return true;
}


#if WITH_EDITOR

void UCartesianPlot::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	static const FName NAME_Datasource = GET_MEMBER_NAME_CHECKED(UCartesianPlot, Datasource);

	if (PropertyChangedEvent.Property != NULL)
	{
		const FName PropName = PropertyChangedEvent.MemberProperty->GetFName();
		if (PropName == NAME_Datasource)
		{
			if (SKantanCartesianChart::IsNullOrValidDatasource(Datasource) == false)
			{
				Datasource = nullptr;
			}
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif


#undef LOCTEXT_NAMESPACE


