// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "KantanPieChartBase.h"
#include "PieChart.generated.h"


/*
A pie chart widget, which can be hooked up to any datasource object implementing IKantanCategoryDatasourceInterface.
*/
UCLASS(Meta = (DisplayName = "Pie Chart (Advanced)"))
class KANTANCHARTSUMG_API UPieChart : public UKantanPieChartBase
{
	GENERATED_BODY()

public:
	UPieChart(FObjectInitializer const&);

public:
	/*
	Sets the data source - the object must implement IKantanCategoryDatasourceInterface for this call to succeed.
	*/
	UFUNCTION(BlueprintCallable, Category = "Data")
	bool SetDatasource(UObject* InDatasource);

public:
	// UWidget interface
	virtual void SynchronizeProperties() override;
	// End of UWidget interface

protected:
	virtual UObject* InternalGetDatasource() override
	{
		return Datasource;
	}

#if WITH_EDITOR
	virtual bool UseDatasourceAtDesignTime() const override
	{
		// If Datasource property is set at design time, then use it
		return true;
	}

public:
	// UWidget interface
	virtual const FSlateBrush* GetEditorIcon() override;
	// End UWidget interface
#endif

protected:
	/*
	Datasource for the chart. Must implement IKantanCategoryDatasourceInterface.
	Generally you should not set this directly in the editor, but through a call to SetDatasource.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "Config", Meta = (DisplayName = "Datasource"))
	class UObject* Datasource;
	// @TODO: EditAnywhere, - Ideally we have a customization that allows this to be bound to a property on the outer UUserWidget.
	// Could be done with the PropName, PropNameDelegate pattern, but this allows a static value to be set, which doesn't make much sense in this context.
};


