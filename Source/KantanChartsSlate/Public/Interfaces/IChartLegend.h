// Copyright (C) 2015-2017 Cameron Angus. All Rights Reserved.

#pragma once


namespace KantanCharts {

	class ICartesianChart;

	class IChartLegend
	{
	public:
		virtual TSharedRef< SWidget > AsWidget() = 0;

	public:
		virtual void SetChart(const TSharedPtr< ICartesianChart >& InChart) = 0;
		virtual void SetMargins(FMargin const& InMargins) = 0;
		virtual void SetSeriesPadding(FMargin const& InPadding) = 0;
		virtual void SetShowDataStyle(bool bShow) = 0;
		virtual void SetBackgroundOverride(const FSlateBrush* Background) = 0;
		virtual void SetFontSizeOverride(int32 FontSize) = 0;
	};

}

