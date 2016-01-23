// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

#pragma once

#include "SKantanChart.h"
#include "CategoryData.h"
#include "KantanCategoryStyle.h"


/** A base class for category chart widgets. */
class KANTANCHARTSSLATE_API SKantanCategoryChart : public SKantanChart
{
public:
	SLATE_BEGIN_ARGS(SKantanCategoryChart):
		_UpdateTickRate(0.0f)
		, _Datasource(nullptr)
		{}

		SLATE_ARGUMENT(class UObject*, Datasource)
		
		/** Rate at which this widget is ticked when sleeping in seconds */
		SLATE_ARGUMENT(float, UpdateTickRate)

	SLATE_END_ARGS()

	/**
	 * Construct the widget
	 * 
	 * @param InArgs   A declaration from which to construct the widget
	 */
	void Construct(const FArguments& InArgs);

	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	bool SetDatasource(UObject* InDatasource);
	void SetUseAutoPerCategoryStyles(bool bEnabled);
	void SetCategoryStylesList(TArray< FKantanCategoryStyle > const& Styles);
	void SetManualCategoryStyleMappings(TMap< FName, FName > const& Mappings);
	void ResetCategoryStyleMappings();

	int32 GetNumCategories() const;
	FName GetCategoryId(int32 Index) const;
	FText GetCategoryLabel(int32 Index) const;
	float GetCategoryValue(int32 Index) const;

	FKantanCategoryStyle const& GetCategoryStyle(FName CatId) const;

	static bool IsValidDatasource(UObject* Source);
	static bool IsNullOrValidDatasource(UObject* Source);

protected:
	virtual void OnActiveTick(double InCurrentTime, float InDeltaTime) override;

	inline const FKantanCategoryStyle* FindCategoryStyle(FName CatStyleId) const
	{
		return CategoryStyles.FindByPredicate([CatStyleId](FKantanCategoryStyle const& Style)
		{
			return Style.CategoryStyleId == CatStyleId;
		});
	}

	void UpdateDynamicCategoryStyleMappings();
	FName GetNextCategoryStyle() const;

public:
	// FGCObject implementation
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	//

protected:
	UObject* Datasource;
	FCategoryDataSnapshot DataSnapshot;

	bool bAutoPerCategoryStyles;
	TArray< FKantanCategoryStyle > CategoryStyles;

	// This maps from category id to category style id.
	// Whenever we poll the Datasource, this gets updated to include mappings for any
	// categories not already present.
	TMap< FName, FName > CategoryStyleMapping;
};

