// Copyright (C) 2015-2017 Cameron Angus. All Rights Reserved.

#include "KantanCartesianDatasourceInterface.h"


int32 IKantanCartesianDatasourceInterface::GetNumSeries_Implementation() const
{
	return 0;
}

FName IKantanCartesianDatasourceInterface::GetSeriesId_Implementation(int32 SeriesIdx) const
{
	return NAME_None;
}

FText IKantanCartesianDatasourceInterface::GetSeriesName_Implementation(int32 SeriesIdx) const
{
	return{};
}

TArray< FKantanCartesianDatapoint > IKantanCartesianDatasourceInterface::GetSeriesDatapoints_Implementation(int32 SeriesIdx) const
{
	return{};
}


