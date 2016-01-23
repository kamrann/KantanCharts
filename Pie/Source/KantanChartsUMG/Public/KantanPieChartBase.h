// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "KantanCategoryChart.h"
#include "SKantanPieChart.h"
#include "KantanPieChartStyle.h"
#include "KantanPieChartBase.generated.h"


UCLASS(Abstract)
class KANTANCHARTSUMG_API UKantanPieChartBase : public UKantanCategoryChart
{
	GENERATED_BODY()

public:
	// Style
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style", meta = (DisplayName = "Style"))
	FKantanPieChartStyle WidgetStyle;

	// Material used to draw pie
	// @TODO: Perhaps this should be part of FKantanPieChartStyle?
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Appearance")
	class UMaterialInterface* PieMaterial;

public:
	UKantanPieChartBase(FObjectInitializer const&);

public:
	// UWidget interface
	virtual void SynchronizeProperties() override;
	// End of UWidget interface

protected:
	// UWidget interface
	virtual TSharedRef< SWidget > RebuildWidget() override;
	// End of UWidget interface

	inline TSharedPtr< SKantanPieChart > GetPieChart()
	{
		return StaticCastSharedPtr< SKantanPieChart >(MyChart);
	}
};


