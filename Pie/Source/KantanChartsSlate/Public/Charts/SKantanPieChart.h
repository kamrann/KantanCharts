// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Charts/SKantanCategoryChart.h"
#include "KantanPieChartStyle.h"
#include "Style/KantanChartsStyleSet.h"

#include "UObject/StrongObjectPtr.h"


/** A pie chart widget. */
class KANTANCHARTSSLATE_API SKantanPieChart :
	public SKantanCategoryChart
{
public:
	~SKantanPieChart();

	SLATE_BEGIN_ARGS(SKantanPieChart) :
		_Style(&FKantanChartsStyleSet::Get().GetWidgetStyle< FKantanPieChartStyle >("KantanPieChartStyle_Default"))
		, _Datasource(nullptr)
		, _UpdateTickRate(0.0f)
		, _PieMaterial(nullptr)
	{}

		SLATE_STYLE_ARGUMENT(FKantanPieChartStyle, Style)

		SLATE_ARGUMENT(class UObject*, Datasource)
		SLATE_ARGUMENT(float, UpdateTickRate)

		SLATE_ARGUMENT(UMaterialInterface*, PieMaterial)
			
	SLATE_END_ARGS()

	/**
	 * Construct the widget
	 * 
	 * @param InArgs   A declaration from which to construct the widget
	 */
	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
		
	void SetStyle(const FKantanPieChartStyle* InStyle);
	// @TODO: Any real need for this to be dynamically updateable?
	void SetPieMaterial(UMaterialInterface* InMaterial);

protected:
	virtual const FKantanPieChartStyle* GetChartStyle() const override
	{
		return static_cast<const FKantanPieChartStyle*>(Style);
	}

	virtual float GetChartAreaSize(EChartContentArea::Type Area, EAxis::Type ReqComp, FVector2D const& KnownPlotSize) const override;
	virtual int32 DrawChartArea(
		EChartContentArea::Type Area,
		const FPaintArgs& Args,
		const FGeometry& Geometry,
		const FGeometry& PlotSpaceGeometry,
		const FSlateRect& MyClippingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled
		) const override;
	virtual void OnActiveTick(double InCurrentTime, float InDeltaTime) override;

	inline bool IsMaterialValid() const
	{
		return PieMaterial != nullptr;
	}

private:
	/** The style */
	const FKantanPieChartStyle* Style;

	TStrongObjectPtr< UMaterialInterface > PieMaterial;
	TArray< TStrongObjectPtr< UMaterialInstanceDynamic > > SegmentMaterialInstances;

	typedef TSharedPtr< class FKantanPieElement, ESPMode::ThreadSafe > FPieElementPtr;
	FPieElementPtr PieElement;
};
