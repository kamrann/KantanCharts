// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

#pragma once

#include "SKantanCategoryChart.h"
#include "KantanBarChartStyle.h"
#include "KantanChartsStyleSet.h"


UENUM(BlueprintType)
enum class EKantanBarChartOrientation : uint8 {
	Vertical,
	Horizontal,
};

UENUM(BlueprintType)
enum class EKantanBarLabelPosition : uint8 {
	NoLabels,
	Standard,
	Overlaid,
};

UENUM(BlueprintType)
enum class EKantanBarValueExtents : uint8 {
	NoValueLines,
	ZeroLineOnly,
	ZeroAndMaxLines,
};


/** A bar chart widget. */
class KANTANCHARTSSLATE_API SKantanBarChart : public SKantanCategoryChart
{
public:
	SLATE_BEGIN_ARGS(SKantanBarChart) :
		_Style(&FKantanChartsStyleSet::Get().GetWidgetStyle< FKantanBarChartStyle >("KantanBarChartStyle_Default"))
		, _Datasource(nullptr)
		, _UpdateTickRate(0.0f)
		, _Orientation(EKantanBarChartOrientation::Vertical)
		, _MaxBarValue(0.0f)
		, _LabelPosition(EKantanBarLabelPosition::Standard)
		, _BarToGapRatio(3.0f)
		, _DrawCategoryBoundaries(true)
		, _ValueExtentsDisplay(EKantanBarValueExtents::ZeroLineOnly)
	{}

		SLATE_STYLE_ARGUMENT(FKantanBarChartStyle, Style)

		SLATE_ARGUMENT(class UObject*, Datasource)
		SLATE_ARGUMENT(float, UpdateTickRate)

		SLATE_ARGUMENT(EKantanBarChartOrientation, Orientation)
		SLATE_ARGUMENT(float, MaxBarValue)
		SLATE_ARGUMENT(EKantanBarLabelPosition, LabelPosition)
		SLATE_ARGUMENT(float, BarToGapRatio)
		SLATE_ARGUMENT(bool, DrawCategoryBoundaries)
		SLATE_ARGUMENT(EKantanBarValueExtents, ValueExtentsDisplay)
		
	SLATE_END_ARGS()

	/**
	 * Construct the widget
	 * 
	 * @param InArgs   A declaration from which to construct the widget
	 */
	void Construct(const FArguments& InArgs);

	void SetStyle(const FKantanBarChartStyle* InStyle);
	void SetOrientation(EKantanBarChartOrientation InOrientation);
	void SetMaxBarValue(float InMax);
	void SetLabelPosition(EKantanBarLabelPosition InPosition);
	void SetBarToGapRatio(float InRatio);
	void SetDrawCategoryBoundaries(bool InDraw);
	void SetValueExtentsDisplay(EKantanBarValueExtents InExtents);
	void SetValueAxisConfig(FCartesianAxisConfig const& InConfig);

protected:
	// Map a box origin from chart space (x across categories, y up bars) to local widget geometry space (x right, y down)
	inline FVector2D MapChartToLocal(FVector2D const& InOrigin, FVector2D const& InBoxSize, FVector2D const& InChartSize) const
	{
		return Orientation == EKantanBarChartOrientation::Vertical ?
			FVector2D(InOrigin.X, InChartSize.Y - InOrigin.Y - InBoxSize.Y) :
			FVector2D(InOrigin.Y, InOrigin.X);
	}

	inline FVector2D MapChartToLocal_Size(FVector2D const& InSize) const
	{
		return Orientation == EKantanBarChartOrientation::Vertical ?
			InSize :
			FVector2D(InSize.Y, InSize.X);
	}

	inline float CalcBarWidthProportion(int32 NumBars) const
	{
		return 1.0f / (NumBars * (1.0f + 1.0f / BarToGapRatio));
	}

	inline float CalcBarWidth(int32 NumBars, float ChartXAvailableWidth) const
	{
		return ChartXAvailableWidth * CalcBarWidthProportion(NumBars);
	}

	FCartesianAxisRange DetermineValueRange() const;
	AxisUtil::FAxisTransform DetermineValueAxisTransform(FVector2D const& PlotSize) const;
	void DetermineCategoryLabelRequirements(float InBarWidth, float& ReqSize, float& Rotation) const;

	virtual const FKantanBarChartStyle* GetChartStyle() const override
	{
		return static_cast<const FKantanBarChartStyle*>(Style);
	}

	virtual float GetChartAreaSize(EChartContentArea::Type Area, EAxis::Type ReqComp, FVector2D const& KnownPlotSize) const override;
	
	FFloatRoundingLevel DetermineValueAxisRoundingLevel(FGeometry const& PlotSpaceGeometry) const;
	AxisUtil::FAxisMarkerData DetermineValueAxisMarkerData(FGeometry const& PlotSpaceGeometry) const;

	void DrawCategoryAxis(
		EChartContentArea::Type Area,
		const FPaintArgs& Args,
		const FGeometry& Geometry,
		const FGeometry& PlotSpaceGeometry,
		const FSlateRect& MyClippingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled
		) const;

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

private:
	enum BarChartLayers {
		MaxValueLine = 0,
		BarInterior = 1,
		BarOutline = 2,
		CategoryAxis = 3,
		ZeroLine = 3,
		ValueAxis = 3,
	};

	/** The style */
	const FKantanBarChartStyle* Style;

	// Chart orientation
	EKantanBarChartOrientation Orientation;

	// Fixed maximum value for the bar chart (if 0 bars are normalized against the highest value at the time)
	float MaxBarValue;

	// Positioning of bar labels
	EKantanBarLabelPosition LabelPosition;

	// Ratio of the thickness of a bar to the thickness of gaps between bars
	float BarToGapRatio;

	// Whether or not to draw markers along the zero axis marking category boundaries
	bool bDrawCategoryBoundaries;

	// Whether to draw lines at the zero and max value positions
	EKantanBarValueExtents ValueExtentsDisplay;

	// Configuration of value axis
	FCartesianAxisConfig ValueAxisCfg;

	TSharedPtr< FSlateBrush > BarInteriorBrush;
	TSharedPtr< FSlateBrush > BarBorderBrush;
};

