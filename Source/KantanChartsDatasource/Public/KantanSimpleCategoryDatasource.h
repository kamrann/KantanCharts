// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "SimpleCategoryDatasourceImpl.h"
#include "KantanCategoryDatasourceInterface.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "UObject/TextProperty.h"
#include "KantanSimpleCategoryDatasource.generated.h"


UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class KANTANCHARTSDATASOURCE_API UKantanSimpleCategoryDatasource :
	public UObject
	, public FKantanCategoryDataList
	, public IKantanCategoryDatasourceInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Kantan Charts")
	static UKantanSimpleCategoryDatasource* NewSimpleCategoryDatasource();

public:
	UKantanSimpleCategoryDatasource();

public:
	/*
	Adds a new category, using the given ID. If the ID is already in use, the function will fail.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Add Category With Id"))
	void BP_AddCategoryWithId(FName Id, FText Name, bool& bSuccess);

	/*
	Adds a new category using an auto-generated ID, which is returned.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Add Category"))
	void BP_AddCategory(FText Name, FName& CatId);

	/*
	Removes the category with the given ID.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Remove Category"))
	void BP_RemoveCategory(FName Id, bool& bSuccess);

	/*
	Removes all categories.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Remove All Categories"))
	void BP_RemoveAllCategories();

	/*
	Sets the value for the given category.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Update Category Value"))
	void BP_UpdateCategoryValue(FName Id, float Value, bool& bSuccess);

public:
	// IKantanCategoryDatasourceInterface
	virtual int32 GetNumCategories_Implementation() const override;
	virtual FName GetCategoryId_Implementation(int32 CatIdx) const override;
	virtual FText GetCategoryName_Implementation(int32 CatIdx) const override;
	virtual float GetCategoryValue_Implementation(int32 CatIdx) const override;
	//
};



