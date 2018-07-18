// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "KantanChart.h"
#include "Charts/SKantanCategoryChart.h"
#include "KantanCategoryChart.generated.h"


/*
A mapping from a category id to a category style.
Used when we want to specify manually which style a particular category in the datasource should use.
*/
USTRUCT()
struct FCategoryStyleManualMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Style")
	FName CategoryId;

	UPROPERTY(EditAnywhere, Category = "Style")
	FKantanCategoryStyle Style;

	FCategoryStyleManualMapping() :
		CategoryId(NAME_None),
		Style()
	{}
};


/*
Base UMG class for category charts.
Cannot be instantiated itself.
*/
UCLASS(Abstract)
class KANTANCHARTSUMG_API UKantanCategoryChart : public UKantanChart
{
	GENERATED_BODY()

public:
	/*
	If enabled, each category will be automatically assigned an unused visual style.
	If disabled, all categories will use the default style.
	This can be overridden via ManualStyleMappings.
	*/
	UPROPERTY(EditAnywhere, Category = "Category Styles")
	bool bAutoPerCategoryStyles;

	/*
	The style set which defines the various visual styles available to draw categories.
	*/
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Category Styles")
	class UKantanCategoryStyleSet* CategoryStyleSet;

	/*
	Provides an override to allow specifying a particular style to be assigned to a category with a given ID.
	*/
	UPROPERTY(EditAnywhere, Category = "Category Styles")
	TArray< FCategoryStyleManualMapping > ManualStyleMappings;

public:
	UKantanCategoryChart(FObjectInitializer const&);

public:
	/*
	Set up a manual category style for the given category id. Manual styles will take precedence over
	default and auto styles.
	*/
	UFUNCTION(BlueprintCallable, Category = "Category Styles", Meta = (Keywords = "Kantan Charts"))
	void AddCategoryStyleOverride(FName CategoryId, FLinearColor Color);

public:
	// UWidget interface
	virtual void SynchronizeProperties() override;
	// End of UWidget interface

#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent & PropertyChangedEvent) override;
#endif

protected:
	void UpdateSlateChartStyles();
	TMap< FName, FName > ConvertStyleMappings(TArray< FCategoryStyleManualMapping > const& Mappings);
	FName GenerateCategoryStyleId() const;

#if WITH_EDITOR
	virtual UObject* InternalGetPreviewDatasource() const override;
#endif

protected:
	inline TSharedPtr< SKantanCategoryChart > GetCategoryChart()
	{
		return StaticCastSharedPtr< SKantanCategoryChart >(MyChart);
	}
};


