// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "KantanChart.h"
#include "Charts/SKantanCartesianChart.h"
#include "KantanCartesianTypes.h"
#include "Style/KantanCartesianChartStyle.h"
#include "KantanCartesianChartBase.generated.h"


/*
A mapping from a series id to a series style.
Used when we want to specify manually which style a particular series in the datasource should use.
*/
USTRUCT()
struct FSeriesStyleManualMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Style")
	FName SeriesId;

	UPROPERTY(EditAnywhere, Category = "Style")
	FKantanSeriesStyle Style;

	FSeriesStyleManualMapping() :
		SeriesId(NAME_None),
		Style()
	{}
};


/*
Base class implementation of UMG cartesian chart.
*/
UCLASS(Abstract, HideCategories = ("Preview"))
class KANTANCHARTSUMG_API UKantanCartesianChartBase : public UKantanChart
{
	GENERATED_BODY()

public:
	// Style
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style", Meta = (DisplayName = "Style"))
	FKantanCartesianChartStyle WidgetStyle;

	/*
	Defines the region of cartesian space which is drawn.
	*/
	UPROPERTY(EditAnywhere, Category = "Config")
	FKantanCartesianPlotScale PlotScale;

	/*
	Datapoints are drawn at fixed pixel sizes to maintain consistency and avoid scaling artefacts.
	This allows the desired size to be configured.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	TEnumAsByte< EKantanDataPointSize::Type > DataPointSize;

	/*
	X axis configuration.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", Meta = (DisplayName = "X Axis", ConfigAxisType = "X"))
	FCartesianAxisConfig XAxisCfg;

	/*
	Y axis configuration.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", Meta = (DisplayName = "Y Axis", ConfigAxisType = "Y"))
	FCartesianAxisConfig YAxisCfg;

	/*
	Padding around axis titles.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	FMargin AxisTitlePadding;

	/*
	The style set which defines the various visual styles available to draw series.
	*/
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Series Styles")
	class UKantanSeriesStyleSet* SeriesStyleSet;

	/*
	Provides an override to allow specifying a particular style to be assigned to a series with a given ID.
	*/
	UPROPERTY(EditAnywhere, Category = "Series Styles")
	TArray< FSeriesStyleManualMapping > ManualStyleMappings;

	/*
	Whether to enable antialiasing on chart data lines.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "Config")
	bool bAntialias;

public:
	UKantanCartesianChartBase(FObjectInitializer const&);

public:
	/*
	Set plot scale with a direct scale value plus focal coordinates in cartesian space.
	*/
	UFUNCTION(BlueprintCallable, Category = "Config", Meta = (Keywords = "Kantan Charts"))
	void SetPlotScale(FVector2D const& InScale, FVector2D const& InFocalCoords);

	/*
	Set plot scale using display ranges for each axis.
	*/
	UFUNCTION(BlueprintCallable, Category = "Config", Meta = (Keywords = "Kantan Charts"))
	void SetPlotScaleByRange(FCartesianAxisRange const& InRangeX, FCartesianAxisRange const& InRangeY);

	/*
	Set data point size, in pixels.
	*/
	UFUNCTION(BlueprintCallable, Category = "Config", Meta = (Keywords = "Kantan Charts"))
	void SetDataPointSize(TEnumAsByte< EKantanDataPointSize::Type > InSize);

	/*
	Set X axis configuration.
	*/
	UFUNCTION(BlueprintCallable, Category = "Config", Meta = (Keywords = "Kantan Charts"))
	void SetXAxisConfig(FCartesianAxisConfig const& InCfg);

	/*
	Set Y axis configuration.
	*/
	UFUNCTION(BlueprintCallable, Category = "Config", Meta = (Keywords = "Kantan Charts"))
	void SetYAxisConfig(FCartesianAxisConfig const& InCfg);

	/*
	Set padding around axis titles.
	*/
	UFUNCTION(BlueprintCallable, Category = "Config", Meta = (Keywords = "Kantan Charts"))
	void SetAxisTitlePadding(FMargin const& InPadding);

	/*
	Set up a manual series style for the given series id. Manual styles will take precedence over
	default and auto styles.
	*/
	UFUNCTION(BlueprintCallable, Category = "Series Styles", Meta = (Keywords = "Kantan Charts"))
	void AddSeriesStyleOverride(FName SeriesId, UKantanPointStyle* PointStyle, FLinearColor Color);

public:
	/*
	Enable/disable series by ID.
	*/
	UFUNCTION(BlueprintCallable, Category = "Series", Meta = (Keywords = "Kantan Charts"))
	void EnableSeries(FName Id, bool bEnable);

	/*
	Configure whether points and/or lines should be drawn for the given series.
	*/
	UFUNCTION(BlueprintCallable, Category = "Series", Meta = (Keywords = "Kantan Charts"))
	void ConfigureSeries(FName Id, bool bDrawPoints, bool bDrawLines);

	/*
	Set the drawing style for the given series.
	/
	Disabled in favour of AddSeriesStyleOverride, since the latter is maintained in the UMG widget's
	properties, whereas this was just passed on directly to the slate widget and will not serialize,
	or persist if the slate widget is reconstructed. Perhaps consider changing how EnableSeries/ConfigureSeries
	work in this respect also.

	UFUNCTION(BlueprintCallable, Category = "Series")
	void SetSeriesStyle(FName Id, FName StyleId);
	*/

public:
	// UWidget interface
	virtual void SynchronizeProperties() override;
	// End of UWidget interface

#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent & PropertyChangedEvent) override;
#endif

	inline TSharedPtr< SKantanCartesianChart > GetCartesianChart()
	{
		return StaticCastSharedPtr< SKantanCartesianChart >(MyChart);
	}

protected:
	// UWidget interface
	virtual TSharedRef< SWidget > RebuildWidget() override;
	// End of UWidget interface

	void UpdateSlateChartStyles();
	TMap< FName, FName > ConvertStyleMappings(TArray< FSeriesStyleManualMapping > const& Mappings);
	FName GenerateSeriesStyleId() const;

#if WITH_EDITOR
	virtual void InitPreview() PURE_VIRTUAL(UKantanCartesianChartBase::InitPreview, );
#endif
};


