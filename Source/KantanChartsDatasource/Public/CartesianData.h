// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "IdBasedList.h"
#include "KantanCartesianDatapoint.h"
#include "KantanCartesianDatasourceInterface.h"
#include "UObject/UnrealType.h"
#include "Math/Interval.h"


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

	/* Is bIsAscending is true, it means we can assume that subsequent values are >= previous, and therefore avoid iterating over all datapoints. */
	FFloatInterval GetSeriesRange(int32 Index, EAxis::Type Axis, bool bIsAscending) const
	{
		auto& Series = Elements[Index];

		FFloatInterval Range(FLT_MAX, -FLT_MAX);
		if(Series.Points.Num() > 0)
		{
			const int32 ComponentIdx = Axis == EAxis::X ? 0 : 1;
			if(bIsAscending)
			{
				Range.Min = Series.Points[0].Coords.Component(ComponentIdx);
				Range.Max = Series.Points.Last().Coords.Component(ComponentIdx);
			}
			else
			{
				for(auto& Pnt : Series.Points)
				{
					Range.Min = FMath::Min(Range.Min, Pnt.Coords.Component(ComponentIdx));
					Range.Max = FMath::Max(Range.Max, Pnt.Coords.Component(ComponentIdx));
				}
			}
		}

		return Range;
	}

	FFloatInterval GetDataRange(EAxis::Type Axis, bool bIsAscending, const TArray< int32 >& SeriesToInclude, bool bFromZero = true) const
	{
		FFloatInterval Range = bFromZero ? FFloatInterval(0, 0) : FFloatInterval(FLT_MAX, -FLT_MAX);
		for(auto& SeriesIdx : SeriesToInclude)
		{
			auto SeriesRange = GetSeriesRange(SeriesIdx, Axis, bIsAscending);
			Range.Min = FMath::Min(Range.Min, SeriesRange.Min);
			Range.Max = FMath::Max(Range.Max, SeriesRange.Max);
		}
		return Range;
	}
};


