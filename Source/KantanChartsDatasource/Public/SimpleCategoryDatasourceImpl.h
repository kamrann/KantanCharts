// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "IdBasedList.h"
#include "CategoryData.h"
#include "CoreMinimal.h"


struct KANTANCHARTSDATASOURCE_API FKantanCategoryDataList :
	public FIdBasedList< FKantanCategoryData >
{
	bool AddCategory(FName const& Id, FText const& Name)
	{
		if (Contains(Id))
		{
			return false;
		}

		FKantanCategoryData CatData;
		CatData.Id = Id;
		CatData.Name = Name;

		Elements.Add(CatData);
		return true;
	}

	FName AddCategory(FText const& Name)
	{
		auto AutoId = GenerateId(CategoryIdPrefix);
		AddCategory(AutoId, Name);
		return AutoId;
	}

	bool RemoveCategory(FName const& Id)
	{
		return Remove(Id);
	}

	void RemoveAllCategories()
	{
		Clear();
	}

	bool UpdateCategoryValue(FName const& Id, float Value)
	{
		auto Cat = Find(Id);
		if (Cat)
		{
			Cat->Value = Value;
			return true;
		}
		else
		{
			return false;
		}
	}
	
private:
	static const FString CategoryIdPrefix;
};




