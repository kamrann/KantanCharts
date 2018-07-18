// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "KantanCartesianChartBase.h"
#include "Style/KantanSeriesStyleSet.h"

#include "UObject/ConstructorHelpers.h"

#define LOCTEXT_NAMESPACE "KantanCharts"


UKantanCartesianChartBase::UKantanCartesianChartBase(FObjectInitializer const& OI) : Super(OI)
{
	SKantanCartesianChart::FArguments SlateDefaults;
	WidgetStyle = *SlateDefaults._Style;

	XAxisCfg.LeftBottomAxis.bEnabled = true;
	YAxisCfg.LeftBottomAxis.bEnabled = true;
	PlotScale = FKantanCartesianPlotScale();
	DataPointSize = EKantanDataPointSize::Medium;
	AxisTitlePadding = FMargin(0.0f, 4.0f);
	bAntialias = true;

#if WITH_EDITORONLY_DATA
	// @NOTE: Disabling for cartesian charts for now, would have to make datasource change with 
	// the chart plot region settings, not really worth it.
	bShowDataPreview = false;
#endif

	static ConstructorHelpers::FObjectFinder< UKantanSeriesStyleSet > DefaultSeriesStyles(
		TEXT("KantanSeriesStyleSet'/KantanCharts/Style/SeriesStyles/DefaultSeriesStyleSet.DefaultSeriesStyleSet'")
		);
	SeriesStyleSet = DefaultSeriesStyles.Object;
}

void UKantanCartesianChartBase::SetPlotScale(FVector2D const& InScale, FVector2D const& InFocalCoords)
{
	PlotScale.Type = ECartesianScalingType::FixedScale;
	PlotScale.Scale = InScale;
	PlotScale.FocalCoordinates = InFocalCoords;
	if (MyChart.IsValid())
	{
		GetCartesianChart()->SetPlotScale(PlotScale);
	}
}

void UKantanCartesianChartBase::SetPlotScaleByRange(FCartesianAxisRange const& InRangeX, FCartesianAxisRange const& InRangeY)
{
	PlotScale.Type = ECartesianScalingType::FixedRange;
	PlotScale.RangeX = InRangeX;
	PlotScale.RangeY = InRangeY;
	if (MyChart.IsValid())
	{
		GetCartesianChart()->SetPlotScale(PlotScale);
	}
}

void UKantanCartesianChartBase::SetDataPointSize(TEnumAsByte< EKantanDataPointSize::Type > InSize)
{
	DataPointSize = InSize;
	if (MyChart.IsValid())
	{
		GetCartesianChart()->SetDataPointSize(DataPointSize);
	}
}

void UKantanCartesianChartBase::SetXAxisConfig(FCartesianAxisConfig const& InCfg)
{
	XAxisCfg = InCfg;
	if (MyChart.IsValid())
	{
		GetCartesianChart()->SetXAxisConfig(XAxisCfg);
	}
}

void UKantanCartesianChartBase::SetYAxisConfig(FCartesianAxisConfig const& InCfg)
{
	YAxisCfg = InCfg;
	if (MyChart.IsValid())
	{
		GetCartesianChart()->SetYAxisConfig(YAxisCfg);
	}
}

void UKantanCartesianChartBase::SetAxisTitlePadding(FMargin const& InPadding)
{
	AxisTitlePadding = InPadding;
	if (MyChart.IsValid())
	{
		GetCartesianChart()->SetAxisTitlePadding(AxisTitlePadding);
	}
}

void UKantanCartesianChartBase::AddSeriesStyleOverride(FName SeriesId, UKantanPointStyle* PointStyle, FLinearColor Color)
{
	FSeriesStyleManualMapping NewMapping;
	NewMapping.SeriesId = SeriesId;
	NewMapping.Style.StyleId = GenerateSeriesStyleId();
	NewMapping.Style.PointStyle = PointStyle;
	NewMapping.Style.Color = Color;
	ManualStyleMappings.Add(NewMapping);

	if (MyChart.IsValid())
	{
		UpdateSlateChartStyles();
	}
}

void UKantanCartesianChartBase::EnableSeries(FName Id, bool bEnable)
{
	if (MyChart.IsValid())
	{
		GetCartesianChart()->EnableSeries(Id, bEnable);
	}
}

void UKantanCartesianChartBase::ConfigureSeries(FName Id, bool bDrawPoints, bool bDrawLines)
{
	if (MyChart.IsValid())
	{
		GetCartesianChart()->ConfigureSeries(Id, bDrawPoints, bDrawLines);
	}
}
/*
void UKantanCartesianChartBase::SetSeriesStyle(FName Id, FName StyleId)
{
	if (MyChart.IsValid())
	{
		GetCartesianChart()->SetSeriesStyle(Id, StyleId);
	}
}
*/

void UKantanCartesianChartBase::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	auto CartesianChart = GetCartesianChart();

	UObject* DatasourceToUse = nullptr;
#if WITH_EDITOR
	// In the editor, if at design time then use the preview datasource, unless the chart has a valid
	// datasource available and specifically wants it to be used.
	bool bPreviewing = false;
	if (IsDesignTime() && bShowDataPreview && (UseDatasourceAtDesignTime() == false || InternalGetDatasource() == nullptr))
	{
		DatasourceToUse = InternalGetPreviewDatasource();
		bPreviewing = DatasourceToUse != nullptr;
	}
	else
#endif
	{
		DatasourceToUse = InternalGetDatasource();
	}

	CartesianChart->SetDatasource(DatasourceToUse);

#if WITH_EDITOR
	if (bPreviewing)
	{
		InitPreview();
	}
#endif

	CartesianChart->SetXAxisConfig(XAxisCfg);
	CartesianChart->SetYAxisConfig(YAxisCfg);
	CartesianChart->SetAxisTitlePadding(AxisTitlePadding);
	CartesianChart->SetPlotScale(PlotScale);
	CartesianChart->SetDataPointSize(DataPointSize);
	CartesianChart->SetAntialiasDataLines(bAntialias);

	CartesianChart->SetUseAutoPerSeriesStyles(true);

	UpdateSlateChartStyles();
}

void UKantanCartesianChartBase::UpdateSlateChartStyles()
{
	check(MyChart.IsValid());
	auto CartesianChart = GetCartesianChart();

	TArray< FKantanSeriesStyle > SeriesStyles;
	if (SeriesStyleSet)
	{
		SeriesStyles.Append(SeriesStyleSet->Styles);
	}
	for (auto const& Override : ManualStyleMappings)
	{
		SeriesStyles.Add(Override.Style);
	}
	CartesianChart->SetSeriesStylesList(SeriesStyles);
	CartesianChart->SetManualSeriesStyleMappings(ConvertStyleMappings(ManualStyleMappings));
}

TMap< FName, FName > UKantanCartesianChartBase::ConvertStyleMappings(TArray< FSeriesStyleManualMapping > const& Mappings)
{
	TMap< FName, FName > Result;
	for (auto const& Mp : Mappings)
	{
		// @TODO: Check if the default TMap prevents duplicate keys
		Result.Add(Mp.SeriesId, Mp.Style.StyleId);
	}
	return Result;
}

FName UKantanCartesianChartBase::GenerateSeriesStyleId() const
{
	static const FString AutoStyleIdPrefix = TEXT("AutoStyleId__");

	int32 MaxNumber = 0;
	for (auto const& Mp : ManualStyleMappings)
	{
		FString IdStr = Mp.Style.StyleId.ToString();
		int32 Idx = IdStr.Find(AutoStyleIdPrefix, ESearchCase::CaseSensitive);
		if (Idx != 0)
		{
			continue;
		}

		auto NumStr = IdStr.RightChop(AutoStyleIdPrefix.Len());
		auto Num = FCString::Atoi(*NumStr);
		MaxNumber = FMath::Max(MaxNumber, Num);
	}

	return FName(*FString::Printf(TEXT("%s%i"), *AutoStyleIdPrefix, MaxNumber + 1));
}


TSharedRef< SWidget > UKantanCartesianChartBase::RebuildWidget()
{
	MyChart = SNew(SKantanCartesianChart)
		.Style(&WidgetStyle);
	return MyChart.ToSharedRef();
}


#if WITH_EDITOR

void UKantanCartesianChartBase::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	static const FName NAME_PlotScale = GET_MEMBER_NAME_CHECKED(UKantanCartesianChartBase, PlotScale);
	static const FName NAME_ManualStyleMappings = GET_MEMBER_NAME_CHECKED(UKantanCartesianChartBase, ManualStyleMappings);

	if (PropertyChangedEvent.Property != NULL)
	{
		const FName PropName = PropertyChangedEvent.PropertyChain.GetHead()->GetValue()->GetFName();

		if (PropName == NAME_PlotScale)
		{
			PlotScale.Validate();
		}
		else if (PropName == NAME_ManualStyleMappings)
		{
			// Auto assign a series style id to any newly added mapping
			for (auto& Override : ManualStyleMappings)
			{
				if (Override.Style.StyleId.IsNone())
				{
					Override.Style.StyleId = GenerateSeriesStyleId();
				}
			}
		}
	}

	Super::PostEditChangeChainProperty(PropertyChangedEvent);
}

#endif


#undef LOCTEXT_NAMESPACE


