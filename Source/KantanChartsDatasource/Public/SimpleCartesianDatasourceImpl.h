// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "IdBasedList.h"
#include "KantanCartesianDatapoint.h"
#include "CartesianData.h"
#include "CoreMinimal.h"


struct KANTANCHARTSDATASOURCE_API FKantanSeriesDataList :
	public FIdBasedList< FKantanSeriesData >
{
	bool AddSeries(FName const& Id, FText const& Name)
	{
		if (Contains(Id))
		{
			return false;
		}

		FKantanSeriesData SeriesData;
		SeriesData.Id = Id;
		SeriesData.Name = Name;

		Elements.Add(SeriesData);
		return true;
	}

	FName AddSeries(FText const& Name)
	{
		auto AutoId = GenerateId(SeriesIdPrefix);
		AddSeries(AutoId, Name);
		return AutoId;
	}

	bool RemoveSeries(FName const& Id)
	{
		return Remove(Id);
	}

	void RemoveAllSeries()
	{
		Clear();
	}

	void SetDatapointLimit(int32 Limit = 0)
	{
		check(Limit >= 0);
		if (DatapointLimit == Limit)
		{
		    return;
		}
		DatapointLimit = Limit;

	    for(auto& Elem : Elements)
		{
			int32 const Count = Elem.Points.Num();
			if (Count > DatapointLimit)
			{
				auto Copy = MoveTemp(Elem.Points);
				Elem.Points.SetNumUninitialized(DatapointLimit);
				FMemory::Memcpy(Elem.Points.GetData(), Copy.GetData(), DatapointLimit * sizeof(Elem.Points[0]));
			}
		}
	}

	bool AddDatapoint(FName const& SeriesId, FVector2D const& Point)
	{
		auto S = Find(SeriesId);
		if (S)
		{
			FKantanCartesianDatapoint Datapoint;
			Datapoint.Coords = Point;
			int32 const CurrentCount = S->Points.Num();
			if (DatapointLimit != 0 && CurrentCount == DatapointLimit)
			{
			    FMemory::Memmove(S->Points.GetData(), S->Points.GetData() + 1, (DatapointLimit - 1) * sizeof(S->Points[0]));
				S->Points.SetNumUninitialized(CurrentCount - 1, false);
			}
			S->Points.Add(Datapoint);
			return true;
		}

		return false;
	}

	bool ClearSeriesData(FName const& SeriesId)
	{
		auto S = Find(SeriesId);
		if(S)
		{
			S->Points.Reset();
			return true;
		}

		return false;
	}

	void ClearAllData()
	{
		for(auto& Elem : Elements)
		{
			Elem.Points.Reset();
		}
	}
	
private:
	int32 DatapointLimit = 0;

	static const FString SeriesIdPrefix;
};



