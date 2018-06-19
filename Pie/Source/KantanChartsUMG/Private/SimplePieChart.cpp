// Fill out your copyright notice in the Description page of Project Settings.

#include "KantanChartsUMG.h"
#include "SimplePieChart.h"
#include "KantanSimpleCategoryDatasource.h"

#define LOCTEXT_NAMESPACE "KantanCharts"


USimplePieChart::USimplePieChart(FObjectInitializer const& OI) : Super(OI)
{

}

void USimplePieChart::BP_AddCategoryWithId(FName Id, FText Name, bool& bSuccess)
{
	bSuccess = AddCategory(Id, Name);
}

void USimplePieChart::BP_AddCategory(FText Name, FName& CatId)
{
	CatId = AddCategory(Name);
}

void USimplePieChart::BP_RemoveCategory(FName Id, bool& bSuccess)
{
	bSuccess = RemoveCategory(Id);
}

void USimplePieChart::BP_RemoveAllCategories()
{
	RemoveAllCategories();
}

void USimplePieChart::BP_UpdateCategoryValue(FName Id, float Value, bool& bSuccess)
{
	bSuccess = UpdateCategoryValue(Id, Value);
}

// IKantanCategoryDatasourceInterface
int32 USimplePieChart::GetNumCategories_Implementation() const
{
	return Elements.Num();
}

FName USimplePieChart::GetCategoryId_Implementation(int32 CatIdx) const
{
	return Elements[CatIdx].Id;
}

FText USimplePieChart::GetCategoryName_Implementation(int32 CatIdx) const
{
	return Elements[CatIdx].Name;
}

float USimplePieChart::GetCategoryValue_Implementation(int32 CatIdx) const
{
	return Elements[CatIdx].Value;
}
//


#if WITH_EDITOR

const FSlateBrush* USimplePieChart::GetEditorIcon()
{
	return FKantanChartsStyleSet::Get().GetBrush("Widget.KantanPieChart");
}

#endif



#undef LOCTEXT_NAMESPACE


