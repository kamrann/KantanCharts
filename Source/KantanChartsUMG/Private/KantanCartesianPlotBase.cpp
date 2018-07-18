// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "KantanCartesianPlotBase.h"

#include "UObject/Package.h"

#define LOCTEXT_NAMESPACE "KantanCharts"


UKantanCartesianPlotBase::UKantanCartesianPlotBase(FObjectInitializer const& OI) : Super(OI)
{

}


#if WITH_EDITOR

UObject* UKantanCartesianPlotBase::InternalGetPreviewDatasource() const
{
	return FindObject< UObject >(
		::GetTransientPackage(),
		TEXT("PreviewCartesianPlotDatasource")
		);
}

void UKantanCartesianPlotBase::InitPreview()
{
	auto CartesianChart = GetCartesianChart();
	auto NumSeries = CartesianChart->GetNumSeries();
	for (int32 Idx = 0; Idx < NumSeries; ++Idx)
	{
		auto Id = CartesianChart->GetSeriesId(Idx);
		CartesianChart->EnableSeries(Id, true);
		CartesianChart->ConfigureSeries(Id, true, false);
	}
}

#endif


#undef LOCTEXT_NAMESPACE


