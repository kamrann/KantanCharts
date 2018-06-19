// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


// @TODO: Since no longer reflected, should just reimplement this using TMap< FName, ElementType >
template < typename ElementType >
struct FIdBasedList
{
	TArray< ElementType > Elements;

	inline ElementType* Find(FName const& Id)
	{
		return Elements.FindByPredicate([Id](ElementType const& Elem)
		{
			return Elem.Id == Id;
		});
	}

	inline const ElementType* Find(FName const& Id) const
	{
		return Elements.FindByPredicate([Id](ElementType const& Elem)
		{
			return Elem.Id == Id;
		});
	}

	inline bool Contains(FName const& Id) const
	{
		return Find(Id) != nullptr;
	}

	inline bool Remove(FName const& Id)
	{
		auto Idx = Elements.IndexOfByPredicate([Id](ElementType const& Elem)
		{
			return Elem.Id == Id;
		});

		if (Idx != INDEX_NONE)
		{
			Elements.RemoveAt(Idx);
			return true;
		}

		return false;
	}

	inline void Clear()
	{
		Elements.Reset();
	}

	FName GenerateId(FString const& Prefix) const
	{
		int32 Num = 0;
		FName Id = *(Prefix + TEXT("__") + FString::FromInt(Num));
		while (Contains(Id))
		{
			++Num;
			Id = *(Prefix + TEXT("__") + FString::FromInt(Num));
		}

		return Id;
	}
};


