// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "Components/Widget.h"
#include "Charts/SKantanChartLegend.h"
#include "KantanChartLegend.generated.h"


class UKantanCartesianChartBase;


/*
Legend widget for a Cartesian chart/Time series plot.
*/
UCLASS(Meta = (DisplayName = "Chart Legend"))
class KANTANCHARTSUMG_API UKantanChartLegend : public UWidget
{
	GENERATED_BODY()

public:
	/*
	Defines margins around the legend.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	FMargin Margins;

	/*
	Padding size around each series entry.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	FMargin SeriesPadding;

	/*
	Whether or not to display the series style alongside the name.
	/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	bool bShowDataStyle;
	*/

	/*
	Background brush to use.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	FSlateBrush Background;

	/*
	Font size to use. If 0, chart font size is used.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	int32 FontSize;

	UPROPERTY(BlueprintReadOnly, Category = "Config")
	TWeakObjectPtr< UKantanCartesianChartBase > Chart;

public:
	UKantanChartLegend(FObjectInitializer const&);

public:
	/*
	Set legend margins.
	*/
	UFUNCTION(BlueprintCallable, Category = "Config", Meta = (Keywords = "Kantan Charts"))
	void SetMargins(FMargin const& InMargins);

	/*
	Set series padding.
	*/
	UFUNCTION(BlueprintCallable, Category = "Config", Meta = (Keywords = "Kantan Charts"))
	void SetSeriesPadding(FMargin const& InPadding);

	/*
	Show data style or not?
	/
	UFUNCTION(BlueprintCallable, Category = "Config", Meta = (Keywords = "Kantan Charts"))
	void ShowDataStyle(bool bShow);
	*/

	/*
	Set background brush.
	*/
	UFUNCTION(BlueprintCallable, Category = "Config", Meta = (Keywords = "Kantan Charts"))
	void SetBackground(FSlateBrush const& InBrush);

	/*
	Set legend font.
	*/
	UFUNCTION(BlueprintCallable, Category = "Config", Meta = (Keywords = "Kantan Charts"))
	void SetFontSize(int32 InFontSize);

	/*
	Set the chart to associate the legend with.
	*/
	UFUNCTION(BlueprintCallable, Category = "Config", Meta = (Keywords = "Kantan Charts"))
	void SetChart(UKantanCartesianChartBase* InChart);

public:
	// UWidget interface
	virtual void SynchronizeProperties() override;
	// End of UWidget interface

	// UVisual interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	// End of UVisual interface

protected:
	// UWidget interface
	virtual TSharedRef< SWidget > RebuildWidget() override;
	// End of UWidget interface

#if WITH_EDITOR
public:
	// UWidget interface
	virtual const FText GetPaletteCategory() override;
	// End UWidget interface
#endif

protected:
	TSharedPtr< SKantanChartLegend > MyWidget;
};


