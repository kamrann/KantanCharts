// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "Charts/SKantanChartLegend.h"
#include "Charts/SKantanCartesianChart.h"

#include "Framework/Application/SlateApplication.h"
#include "Fonts/FontMeasure.h"


void SKantanChartLegend::Construct(const FArguments& InArgs)
{
	SetMargins(InArgs._Margins);
	SetSeriesPadding(InArgs._SeriesPadding);
	SetShowDataStyle(InArgs._bShowDataStyle);
	SetBackgroundOverride(InArgs._BackgroundOverride);
	SetFontSizeOverride(InArgs._FontSizeOverride);
}

TSharedRef< SWidget > SKantanChartLegend::AsWidget()
{
	return SharedThis(this);
}

void SKantanChartLegend::SetChart(const TSharedPtr< KantanCharts::ICartesianChart >& InChart)
{
	ChartPtr = InChart; //StaticCastSharedPtr< SKantanCartesianChart >(InChart);
}

void SKantanChartLegend::SetMargins(FMargin const& InMargins)
{
	Margins = InMargins;
}

void SKantanChartLegend::SetSeriesPadding(FMargin const& InPadding)
{
	Padding = InPadding;
}

void SKantanChartLegend::SetShowDataStyle(bool bShow)
{
	bShowDataStyle = bShow;
}

void SKantanChartLegend::SetBackgroundOverride(const FSlateBrush* Background)
{
	BackgroundOverride = Background;
}

void SKantanChartLegend::SetFontSizeOverride(int32 FontSize)
{
	FontSizeOverride = FontSize;
}

FVector2D SKantanChartLegend::ComputeDesiredSize(float) const
{
	auto Chart = ChartPtr.Pin();
	if(!Chart.IsValid())
	{
		return FVector2D(150, 30);
	}

	auto FontInfo = GetLegendFont();
	auto FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

	auto const& Snapshot = Chart->GetCurrentSnapshot();
	FVector2D ReqSize(0, 0);
	for(auto Entry : Snapshot.Elements)
	{
		FText Label = Entry.Name;
		FVector2D LabelExtent = FontMeasureService->Measure(Label, FontInfo);
		LabelExtent += Padding.GetDesiredSize();
		ReqSize.Y += LabelExtent.Y;
		ReqSize.X = FMath::Max(LabelExtent.X, ReqSize.X);
	}

	ReqSize += Margins.GetDesiredSize();
	return ReqSize;
}

int32 SKantanChartLegend::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	// Pre-snap the clipping rect to try and reduce common jitter, since the padding is typically only a single pixel.
	FSlateRect SnappedClippingRect = FSlateRect(FMath::RoundToInt(MyClippingRect.Left), FMath::RoundToInt(MyClippingRect.Top), FMath::RoundToInt(MyClippingRect.Right), FMath::RoundToInt(MyClippingRect.Bottom));

	auto FontInfo = GetLegendFont();

	auto Chart = ChartPtr.Pin();
	if(!Chart.IsValid())
	{
		auto Label = FText::FromString(TEXT("[Legend]"));
		FSlateDrawElement::MakeText(
			OutDrawElements,
			LayerId,
			AllottedGeometry.ToPaintGeometry(),
			Label,
			FontInfo,
			//SnappedClippingRect,
			ESlateDrawEffect::None
			);
		return LayerId;
	}

	auto ChartStyle = Chart->GetChartStyle();

	auto Brush = BackgroundOverride;
	if(Brush == nullptr)
	{
		Brush = &ChartStyle->Background;
	}
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(),
		Brush,
		//SnappedClippingRect,
		ESlateDrawEffect::None,
		Brush->TintColor.GetColor(InWidgetStyle)
		);
	++LayerId;
	
	auto FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

	auto const& Snapshot = Chart->GetCurrentSnapshot();
	FVector2D Offset(Margins.Left, Margins.Top);
	for(auto Entry : Snapshot.Elements)
	{
		auto Id = Entry.Id;
		if(!Chart->IsSeriesEnabled(Id))
		{
			continue;
		}

		auto const& SeriesStyle = Chart->GetSeriesStyle(Id);

		FText Label = Entry.Name;
		FVector2D LabelExtent = FontMeasureService->Measure(Label, FontInfo);

		auto TextGeom = AllottedGeometry.MakeChild(
			LabelExtent,
			FSlateLayoutTransform(Offset + FVector2D(Padding.Left, Padding.Top))
			);
		FSlateDrawElement::MakeText(
			OutDrawElements,
			LayerId,
			TextGeom.ToPaintGeometry(),
			Label,
			FontInfo,
			//SnappedClippingRect,
			ESlateDrawEffect::None,
			SeriesStyle.Color
			);

		Offset.Y += LabelExtent.Y + Padding.GetDesiredSize().Y;
	}

	return LayerId;
}

FSlateFontInfo SKantanChartLegend::GetLegendFont() const
{
	auto Chart = ChartPtr.Pin();
	if(!Chart.IsValid())
	{
		auto DefaultFont = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText").Font;
		DefaultFont.Size = 18;
		return DefaultFont;
	}

	// Default to chart font
	auto ChartStyle = Chart->GetChartStyle();
	auto FontInfo = SKantanChart::GetLabelFont(ChartStyle, EKantanChartLabelClass::AxisTitle);

	// But allow size override
	if(FontSizeOverride != 0)
	{
		FontInfo.Size = FontSizeOverride;
	}

	return FontInfo;
}

