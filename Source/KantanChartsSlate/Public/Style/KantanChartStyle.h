// Copyright (C) 2015-2017 Cameron Angus. All Rights Reserved.

#pragma once

//#include "SlateWidgetStyle.h"
//#include "SlateWidgetStyleContainerBase.h"
#include "SlateCore.h"
#include "KantanChartStyle.generated.h"


USTRUCT(BlueprintType)
struct KANTANCHARTSSLATE_API FKantanChartStyle : public FSlateWidgetStyle
{
	GENERATED_BODY()

	/*
	Chart background brush.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FSlateBrush Background;

	/*
	Color used to draw lines of the chart itself (e.g. axis lines).
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FLinearColor ChartLineColor;

	/*
	The default font used for chart text.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FSlateFontInfo BaseFont;

	/*
	Font size override for chart title.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	int32 TitleFontSize;

	/*
	Font size override for axis titles.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	int32 AxisDescriptionFontSize;

	/*
	Font size override for axis values/labels.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	int32 AxisValueFontSize;

	/*
	Color used for chart text.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FLinearColor FontColor;

	FKantanChartStyle() :
		Background()
		, ChartLineColor(FLinearColor::White)
		, BaseFont()
		, TitleFontSize(0)
		, AxisDescriptionFontSize(0)
		, AxisValueFontSize(0)
		, FontColor(FLinearColor::White)
	{}

	static inline FSlateFontInfo GetOverriddenFont(FSlateFontInfo const& Base, int32 SizeOverride)
	{
		FSlateFontInfo Font(Base);
		if (SizeOverride != 0)
		{
			Font.Size = SizeOverride;
		}
		return Font;
	}

	inline FSlateFontInfo GetTitleFont() const
	{
		return GetOverriddenFont(BaseFont, TitleFontSize);
	}

	inline FSlateFontInfo GetAxisDescriptionFont() const
	{
		return GetOverriddenFont(BaseFont, AxisDescriptionFontSize);
	}

	inline FSlateFontInfo GetAxisValueFont() const
	{
		return GetOverriddenFont(BaseFont, AxisValueFontSize);
	}
};

