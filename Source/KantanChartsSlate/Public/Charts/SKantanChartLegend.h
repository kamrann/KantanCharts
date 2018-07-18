// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "Interfaces/IChartLegend.h"
#include "Style/KantanChartStyle.h"

#include "Widgets/SLeafWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"


class SKantanCartesianChart;


/** A simple widget to display a chart legend. */
class KANTANCHARTSSLATE_API SKantanChartLegend :
	public SLeafWidget
	, public KantanCharts::IChartLegend
{
public:
	SLATE_BEGIN_ARGS(SKantanChartLegend):
		_bShowDataStyle(true)
		, _BackgroundOverride(nullptr)
		, _FontSizeOverride(0)
		{}

		//SLATE_STYLE_ARGUMENT(FKantanCartesianChartStyle, Style)

		SLATE_ARGUMENT(FMargin, Margins)
		SLATE_ARGUMENT(FMargin, SeriesPadding)
		SLATE_ARGUMENT(bool, bShowDataStyle)
		SLATE_ARGUMENT(const FSlateBrush*, BackgroundOverride)
		SLATE_ARGUMENT(int32, FontSizeOverride)
	SLATE_END_ARGS()

	/**
	 * Construct the widget
	 * 
	 * @param InArgs   A declaration from which to construct the widget
	 */
	void Construct(const FArguments& InArgs);

	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	virtual TSharedRef< SWidget > AsWidget() override;

	virtual void SetChart(const TSharedPtr< KantanCharts::ICartesianChart >& InChart) override;
	virtual void SetMargins(FMargin const& InMargins) override;
	virtual void SetSeriesPadding(FMargin const& InPadding) override;
	virtual void SetShowDataStyle(bool bShow) override;
	virtual void SetBackgroundOverride(const FSlateBrush* Background) override;
	virtual void SetFontSizeOverride(int32 FontSize) override;

protected:
	FSlateFontInfo GetLegendFont() const;

protected:
	FMargin Margins;
	FMargin Padding;
	bool bShowDataStyle;
	const FSlateBrush* BackgroundOverride;
	int32 FontSizeOverride;

	//TWeakPtr< SKantanCartesianChart > ChartPtr;
	TWeakPtr< KantanCharts::ICartesianChart > ChartPtr;
};

