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

	bool AddDatapoint(FName const& SeriesId, FVector2D const& Point)
	{
		auto S = Find(SeriesId);
		if (S)
		{
			FKantanCartesianDatapoint Datapoint;
			Datapoint.Coords = Point;
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
	static const FString SeriesIdPrefix;
};



