// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "SimpleBarChart.h"

#define LOCTEXT_NAMESPACE "KantanCharts"


USimpleBarChart::USimpleBarChart(FObjectInitializer const& OI) : Super(OI)
{

}

void USimpleBarChart::BP_AddCategoryWithId(FName Id, FText Name, bool& bSuccess)
{
	bSuccess = AddCategory(Id, Name);
}

void USimpleBarChart::BP_AddCategory(FText Name, FName& CatId)
{
	CatId = AddCategory(Name);
}

void USimpleBarChart::BP_RemoveCategory(FName Id, bool& bSuccess)
{
	bSuccess = RemoveCategory(Id);
}

void USimpleBarChart::BP_RemoveAllCategories()
{
	RemoveAllCategories();
}

void USimpleBarChart::BP_UpdateCategoryValue(FName Id, float Value, bool& bSuccess)
{
	bSuccess = UpdateCategoryValue(Id, Value);
}


// IKantanCategoryDatasourceInterface
int32 USimpleBarChart::GetNumCategories_Implementation() const
{
	return Elements.Num();
}

FName USimpleBarChart::GetCategoryId_Implementation(int32 CatIdx) const
{
	return Elements[CatIdx].Id;
}

FText USimpleBarChart::GetCategoryName_Implementation(int32 CatIdx) const
{
	return Elements[CatIdx].Name;
}

float USimpleBarChart::GetCategoryValue_Implementation(int32 CatIdx) const
{
	return Elements[CatIdx].Value;
}
//



#undef LOCTEXT_NAMESPACE


