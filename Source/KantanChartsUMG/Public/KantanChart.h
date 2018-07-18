// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "Components/Widget.h"
#include "Charts/SKantanChart.h"
#include "KantanChart.generated.h"


/*
Base UMG chart class.
Cannot be instantiated itself.
*/
UCLASS(Abstract)
class KANTANCHARTSUMG_API UKantanChart : public UWidget
{
	GENERATED_BODY()

public:
	/*
	Defines margins around the chart. The background brush will include these margins, but the chart will be drawn within.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	FMargin Margins;

	/*
	Optional title for the chart.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	FText ChartTitle;

	/*
	Padding around the chart title text.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	FMargin TitlePadding;

	/*
	Update period (in seconds) for retrieving data from datasource.
	A value of 0 will result in the datasource being accessed on every tick.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	float UpdateTickRate;

#if WITH_EDITORONLY_DATA
	/*
	Toggle in-editor display using a preview datasource.
	*/
	UPROPERTY(EditAnywhere, Category = "Preview")
	bool bShowDataPreview;
#endif

public:
	UKantanChart(FObjectInitializer const&);

public:
	/*
	Set chart margins.
	*/
	UFUNCTION(BlueprintCallable, Category = "Config", Meta = (Keywords = "Kantan Charts"))
	void SetMargins(FMargin const& InMargins);

	/*
	Set chart title.
	*/
	UFUNCTION(BlueprintCallable, Category = "Config", Meta = (Keywords = "Kantan Charts"))
	void SetChartTitle(FText const& InTitle);

	/*
	Set title padding.
	*/
	UFUNCTION(BlueprintCallable, Category = "Config", Meta = (Keywords = "Kantan Charts"))
	void SetChartTitlePadding(FMargin const& InPadding);

	/*
	Set the datasource update period.
	*/
	UFUNCTION(BlueprintCallable, Category = "Config", Meta = (Keywords = "Kantan Charts"))
	void SetUpdateTickRate(float InRate);

public:
	// UWidget interface
	virtual void SynchronizeProperties() override;
	// End of UWidget interface

	// UVisual interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	// End of UVisual interface

protected:
	virtual UObject* InternalGetDatasource() PURE_VIRTUAL(UKantanChart::InternalGetDatasource, return nullptr;);

#if WITH_EDITOR
	virtual UObject* InternalGetPreviewDatasource() const PURE_VIRTUAL(UKantanChart::InternalGetPreviewDatasource, return nullptr;);

	// Should the datasource which the chart is configured to use be used even at design time?
	virtual bool UseDatasourceAtDesignTime() const
	{
		return false;
	}
#endif

#if WITH_EDITOR
public:
	// UWidget interface
//	virtual const FSlateBrush* GetEditorIcon() override;
	virtual const FText GetPaletteCategory() override;
	// End UWidget interface
#endif

protected:
	TSharedPtr< SKantanChart > MyChart;
};


