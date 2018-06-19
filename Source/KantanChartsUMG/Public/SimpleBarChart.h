// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "KantanBarChartBase.h"
#include "SimpleCategoryDatasourceImpl.h"
#include "KantanCategoryDatasourceInterface.h"
#include "SimpleBarChart.generated.h"


/*
A simple bar chart widget with integrated data source.
*/
UCLASS(Meta = (DisplayName = "Bar Chart"))
class KANTANCHARTSUMG_API USimpleBarChart:
	public UKantanBarChartBase
	, public FKantanCategoryDataList
	, public IKantanCategoryDatasourceInterface
{
	GENERATED_BODY()

public:
	USimpleBarChart(FObjectInitializer const&);

public:
	/*
	Adds a new category to the chart, using the given ID. If the ID is already in use, the function will fail.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Add Category With Id", Keywords = "Kantan Charts"))
	void BP_AddCategoryWithId(FName Id, FText Name, bool& bSuccess);

	/*
	Adds a new category to the chart using an auto-generated ID, which is returned.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Add Category", Keywords = "Kantan Charts"))
	void BP_AddCategory(FText Name, FName& CatId);

	/*
	Removes the category with the given ID.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Remove Category", Keywords = "Kantan Charts"))
	void BP_RemoveCategory(FName Id, bool& bSuccess);

	/*
	Removes all categories.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Remove All Categories", Keywords = "Kantan Charts"))
	void BP_RemoveAllCategories();

	/*
	Sets the value for the given category.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Update Category Value", Keywords = "Kantan Charts"))
	void BP_UpdateCategoryValue(FName Id, float Value, bool& bSuccess);

public:
	// IKantanCategoryDatasourceInterface
	virtual int32 GetNumCategories_Implementation() const override;
	virtual FName GetCategoryId_Implementation(int32 CatIdx) const override;
	virtual FText GetCategoryName_Implementation(int32 CatIdx) const override;
	virtual float GetCategoryValue_Implementation(int32 CatIdx) const override;
	//

protected:
	virtual UObject* InternalGetDatasource() override
	{
		return this;
	}

#if WITH_EDITOR
	virtual bool UseDatasourceAtDesignTime() const override
	{
		// Never use our own datasource at design time, as it is runtime configured only
		return false;
	}
#endif
};


