// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

#pragma once

#include "IdBasedList.h"
#include "KantanCartesianDatapoint.h"
#include "KantanCartesianDatasourceInterface.h"


struct FKantanSeriesData
{
	FName Id;
	FText Name;
	TArray< FKantanCartesianDatapoint > Points;

	FKantanSeriesData() :
		Id(NAME_None),
		Name(),
		Points()
	{}
};


struct FCartesianDataSnapshot :
	public FIdBasedList< FKantanSeriesData >
{
	void UpdateFromDatasource(UObject* Datasource)
	{
		Clear();
		auto const Count = IKantanCartesianDatasourceInterface::Execute_GetNumSeries(Datasource);
		for (int32 Idx = 0; Idx < Count; ++Idx)
		{
			FKantanSeriesData Series;
			Series.Id = IKantanCartesianDatasourceInterface::Execute_GetSeriesId(Datasource, Idx);
			Series.Name = IKantanCartesianDatasourceInterface::Execute_GetSeriesName(Datasource, Idx);
			Series.Points = IKantanCartesianDatasourceInterface::Execute_GetSeriesDatapoints(Datasource, Idx);
			Elements.Add(Series);
		}
	}
};


