// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "KantanPieChartBase.h"
#include "SimpleCategoryDatasourceImpl.h"
#include "KantanCategoryDatasourceInterface.h"
#include "SimplePieChart.generated.h"


/*
A simple pie chart widget with integrated data source.
*/
UCLASS(Meta = (DisplayName = "Pie Chart"))
class KANTANCHARTSUMG_API USimplePieChart:
	public UKantanPieChartBase
	, public FKantanCategoryDataList
	, public IKantanCategoryDatasourceInterface
{
	GENERATED_BODY()

public:
	USimplePieChart(FObjectInitializer const&);

public:
	/*
	Adds a new category to the chart, using the given ID. If the ID is already in use, the function will fail.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data", Meta = (DisplayName = "Add Category With Id"))
	void BP_AddCategoryWithId(FName Id, FText Name, bool& bSuccess);

	/*
	Adds a new category to the chart using an auto-generated ID, which is returned.
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

public:
	// UWidget interface
	virtual const FSlateBrush* GetEditorIcon() override;
	// End UWidget interface
#endif
};


