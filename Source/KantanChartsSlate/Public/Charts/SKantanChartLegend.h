// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

#pragma once

#include "SLeafWidget.h"
#include "KantanChartStyle.h"


class SKantanCartesianChart;


/** A simple widget to display a chart legend. */
class KANTANCHARTSSLATE_API SKantanChartLegend :
	public SLeafWidget
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

	void SetChart(TSharedPtr< SKantanCartesianChart > InChart);
	void SetMargins(FMargin const& InMargins);
	void SetSeriesPadding(FMargin const& InPadding);
	void SetShowDataStyle(bool bShow);
	void SetBackgroundOverride(const FSlateBrush* Background);
	void SetFontSizeOverride(int32 FontSize);

protected:
	FSlateFontInfo GetLegendFont() const;

protected:
	FMargin Margins;
	FMargin Padding;
	bool bShowDataStyle;
	const FSlateBrush* BackgroundOverride;
	int32 FontSizeOverride;

	TWeakPtr< SKantanCartesianChart > ChartPtr;
};

