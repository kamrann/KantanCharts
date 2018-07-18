// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "IdBasedList.h"
#include "KantanCategoryDatasourceInterface.h"
#include "UObject/UnrealType.h"


struct FKantanCategoryData
{
	FName Id;
	FText Name;
	float Value;

	FKantanCategoryData() :
		Id(NAME_None),
		Name(),
		Value(0.0f)
	{}
};

// Have one of these inside category chart, and update it within ActiveTick.
// Then access only this from within paint methods, never the datasource directly.

// For series data, GetSeriesDatapoints should be modified to also take a previous snapshot timestamp (integer), and
// return the latest one.
// The series data snapshop struct would hold a timestamp, pass this in on update, and set it to the resturned one.
// The datasource should return a flag denoting how to interpret the returned datapoints.
// e.g. Absolute = All points returned; Appended = points returned should just be appended onto previous snapshot.

struct FCategoryDataSnapshot:
	public FIdBasedList< FKantanCategoryData >
{
	void UpdateFromDatasource(UObject* Datasource)
	{
		Clear();
		auto const Count = IKantanCategoryDatasourceInterface::Execute_GetNumCategories(Datasource);
		for (int32 Idx = 0; Idx < Count; ++Idx)
		{
			FKantanCategoryData Cat;
			Cat.Id = IKantanCategoryDatasourceInterface::Execute_GetCategoryId(Datasource, Idx);
			Cat.Name = IKantanCategoryDatasourceInterface::Execute_GetCategoryName(Datasource, Idx);
			Cat.Value = IKantanCategoryDatasourceInterface::Execute_GetCategoryValue(Datasource, Idx);
			Elements.Add(Cat);
		}
	}
};


