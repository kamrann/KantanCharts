// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "KantanSimpleCategoryDatasource.h"
#include "UObject/Package.h"


// @TODO: Should really be in separate cpp file
const FString FKantanCategoryDataList::CategoryIdPrefix = TEXT("KantanSimpleCategory");


UKantanSimpleCategoryDatasource* UKantanSimpleCategoryDatasource::NewSimpleCategoryDatasource()
{
	return NewObject< UKantanSimpleCategoryDatasource >(GetTransientPackage());
}


UKantanSimpleCategoryDatasource::UKantanSimpleCategoryDatasource()
{

}

void UKantanSimpleCategoryDatasource::BP_AddCategoryWithId(FName Id, FText Name, bool& bSuccess)
{
	bSuccess = AddCategory(Id, Name);
}

void UKantanSimpleCategoryDatasource::BP_AddCategory(FText Name, FName& CatId)
{
	CatId = AddCategory(Name);
}

void UKantanSimpleCategoryDatasource::BP_RemoveCategory(FName Id, bool& bSuccess)
{
	bSuccess = RemoveCategory(Id);
}

void UKantanSimpleCategoryDatasource::BP_RemoveAllCategories()
{
	RemoveAllCategories();
}

void UKantanSimpleCategoryDatasource::BP_UpdateCategoryValue(FName Id, float Value, bool& bSuccess)
{
	bSuccess = UpdateCategoryValue(Id, Value);
}

// IKantanCategoryDatasourceInterface
int32 UKantanSimpleCategoryDatasource::GetNumCategories_Implementation() const
{
	return Elements.Num();
}

FName UKantanSimpleCategoryDatasource::GetCategoryId_Implementation(int32 CatIdx) const
{
	return Elements[CatIdx].Id;
}

FText UKantanSimpleCategoryDatasource::GetCategoryName_Implementation(int32 CatIdx) const
{
	return Elements[CatIdx].Name;
}

float UKantanSimpleCategoryDatasource::GetCategoryValue_Implementation(int32 CatIdx) const
{
	return Elements[CatIdx].Value;
}
//





