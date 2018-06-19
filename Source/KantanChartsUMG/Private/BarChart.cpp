// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "BarChart.h"


UBarChart::UBarChart(FObjectInitializer const& OI) : Super(OI)
{
	Datasource = nullptr;
}

bool UBarChart::SetDatasource(UObject* InDatasource)
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


#if WITH_EDITOR

void UBarChart::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	static const FName NAME_Datasource = GET_MEMBER_NAME_CHECKED(UBarChart, Datasource);

	if (PropertyChangedEvent.Property != NULL)
	{
		const FName PropName = PropertyChangedEvent.MemberProperty->GetFName();
		if (PropName == NAME_Datasource)
		{
			if (SKantanCategoryChart::IsNullOrValidDatasource(Datasource) == false)
			{
				Datasource = nullptr;
			}
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif


