// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "Charts/SKantanChart.h"
#include "KantanCartesianTypes.h"	// @TODO: refactor to avoid cartesian specific includes

#include "Layout/SlateRotatedRect.h"
#include "Brushes/SlateColorBrush.h"
#include "Framework/Application/SlateApplication.h"
#include "Fonts/FontMeasure.h"

#define LOCTEXT_NAMESPACE "KantanCharts"


void SKantanChart::Construct(const FArguments& InArgs)
{
	SetMargins(InArgs._Margins);
	SetChartTitle(InArgs._ChartTitle);
	SetChartTitlePadding(FMargin(0.0f, 5.0f));
	SetUpdateTickRate(InArgs._UpdateTickRate);
	SetOnChartMouseDown(InArgs._OnChartMouseDown);
}

TSharedRef< SWidget > SKantanChart::AsWidget()
{
	return SharedThis(this);
}

void SKantanChart::SetMargins(FMargin const& InMargins)
{
	Margins = InMargins;
}

void SKantanChart::SetChartTitle(FText const& InTitle)
{
	ChartTitle = InTitle;
}

void SKantanChart::SetChartTitlePadding(FMargin const& InPadding)
{
	TitlePadding = InPadding;
}

void SKantanChart::SetUpdateTickRate(float InRate)
{
	SetActiveTimerTickRate(InRate);
}

void SKantanChart::SetOnChartMouseDown(KantanCharts::FOnInteractionMouseDown InDelegate)
{
	OnChartMouseDown = InDelegate;
}

FSlateFontInfo SKantanChart::GetLabelFont(const FKantanChartStyle* ChartStyle, EKantanChartLabelClass LabelClass)
{
	FSlateFontInfo FontInfo;
	if (ChartStyle)
	{
		switch (LabelClass)
		{
		case EKantanChartLabelClass::ChartTitle:
			FontInfo = ChartStyle->GetTitleFont();
			break;
		case EKantanChartLabelClass::AxisTitle:
			FontInfo = ChartStyle->GetAxisDescriptionFont();
			break;
		case EKantanChartLabelClass::AxisMarkerLabel:
			FontInfo = ChartStyle->GetAxisValueFont();
			break;
		default:
			FontInfo = ChartStyle->BaseFont;
		}
	}

	if (!FontInfo.HasValidFont())
	{
		auto RequestedSize = FontInfo.Size;

		// Fall back onto font from slate core style
		FontInfo = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText").Font;

		// But try to use the requested font size if non-zero, even if we didn't have a valid font
		if (RequestedSize != 0)
		{
			FontInfo.Size = RequestedSize;
		}
	}

	return FontInfo;
}

FGeometry SKantanChart::MakeInsetGeometry(FGeometry const& WidgetGeometry) const
{
	auto Size = WidgetGeometry.GetLocalSize() - FVector2D(Margins.GetDesiredSize());
	Size = FVector2D(FMath::Max(Size.X, 0.0f), FMath::Max(Size.Y, 0.0f));
	auto ActualCompositeMargins = WidgetGeometry.GetLocalSize() - Size;
	return WidgetGeometry.MakeChild(
		Size,
		FSlateLayoutTransform(FVector2D(Margins.Left, Margins.Top))
		);
}

void SKantanChart::MakeAreaGeometries(FGeometry const& ContentGeometry, FGeometry(&Geoms)[ChartContentAreaCount]) const
{
	auto ChartStyle = GetChartStyle();
	auto FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	auto LabelFont = GetLabelFont(ChartStyle, EKantanChartLabelClass::AxisTitle);
	auto AvailableSize = ContentGeometry.GetLocalSize();

	FVector2D AreaSizes[ChartContentAreaCount];
	AreaSizes[EChartContentArea::Plot] = FVector2D::ZeroVector;

	// Determine X sizes of all areas
	AreaSizes[EChartContentArea::XAxisBottomTitle].X = GetChartAreaSize(EChartContentArea::XAxisBottomTitle, EAxis::X, AreaSizes[EChartContentArea::Plot]);
	AreaSizes[EChartContentArea::XAxisTopTitle].X = GetChartAreaSize(EChartContentArea::XAxisTopTitle, EAxis::X, AreaSizes[EChartContentArea::Plot]);
	AreaSizes[EChartContentArea::YAxisLeftTitle].X = GetChartAreaSize(EChartContentArea::YAxisLeftTitle, EAxis::X, AreaSizes[EChartContentArea::Plot]);
	AreaSizes[EChartContentArea::YAxisRightTitle].X = GetChartAreaSize(EChartContentArea::YAxisRightTitle, EAxis::X, AreaSizes[EChartContentArea::Plot]);
	AreaSizes[EChartContentArea::XAxisBottom].X = GetChartAreaSize(EChartContentArea::XAxisBottom, EAxis::X, AreaSizes[EChartContentArea::Plot]);
	AreaSizes[EChartContentArea::XAxisTop].X = GetChartAreaSize(EChartContentArea::XAxisTop, EAxis::X, AreaSizes[EChartContentArea::Plot]);
	AreaSizes[EChartContentArea::YAxisLeft].X = GetChartAreaSize(EChartContentArea::YAxisLeft, EAxis::X, AreaSizes[EChartContentArea::Plot]);
	AreaSizes[EChartContentArea::YAxisRight].X = GetChartAreaSize(EChartContentArea::YAxisRight, EAxis::X, AreaSizes[EChartContentArea::Plot]);

	AreaSizes[EChartContentArea::Plot].X = AvailableSize.X;

	AreaSizes[EChartContentArea::Plot].X -= AreaSizes[EChartContentArea::YAxisLeftTitle].X;
	AreaSizes[EChartContentArea::Plot].X -= AreaSizes[EChartContentArea::YAxisLeft].X;
	AreaSizes[EChartContentArea::Plot].X -= AreaSizes[EChartContentArea::YAxisRightTitle].X;
	AreaSizes[EChartContentArea::Plot].X -= AreaSizes[EChartContentArea::YAxisRight].X;

	// Now determine Y sizes

	AreaSizes[EChartContentArea::XAxisBottomTitle].Y = GetChartAreaSize(EChartContentArea::XAxisBottomTitle, EAxis::Y, AreaSizes[EChartContentArea::Plot]);
	AreaSizes[EChartContentArea::XAxisTopTitle].Y = GetChartAreaSize(EChartContentArea::XAxisTopTitle, EAxis::Y, AreaSizes[EChartContentArea::Plot]);
	AreaSizes[EChartContentArea::YAxisLeftTitle].Y = GetChartAreaSize(EChartContentArea::YAxisLeftTitle, EAxis::Y, AreaSizes[EChartContentArea::Plot]);
	AreaSizes[EChartContentArea::YAxisRightTitle].Y = GetChartAreaSize(EChartContentArea::YAxisRightTitle, EAxis::Y, AreaSizes[EChartContentArea::Plot]);
	AreaSizes[EChartContentArea::XAxisBottom].Y = GetChartAreaSize(EChartContentArea::XAxisBottom, EAxis::Y, AreaSizes[EChartContentArea::Plot]);
	AreaSizes[EChartContentArea::XAxisTop].Y = GetChartAreaSize(EChartContentArea::XAxisTop, EAxis::Y, AreaSizes[EChartContentArea::Plot]);
	AreaSizes[EChartContentArea::YAxisLeft].Y = GetChartAreaSize(EChartContentArea::YAxisLeft, EAxis::Y, AreaSizes[EChartContentArea::Plot]);
	AreaSizes[EChartContentArea::YAxisRight].Y = GetChartAreaSize(EChartContentArea::YAxisRight, EAxis::Y, AreaSizes[EChartContentArea::Plot]);

	AreaSizes[EChartContentArea::Plot].Y = AvailableSize.Y;

	AreaSizes[EChartContentArea::Plot].Y -= AreaSizes[EChartContentArea::XAxisTopTitle].Y;
	AreaSizes[EChartContentArea::Plot].Y -= AreaSizes[EChartContentArea::XAxisTop].Y;
	AreaSizes[EChartContentArea::Plot].Y -= AreaSizes[EChartContentArea::XAxisBottomTitle].Y;
	AreaSizes[EChartContentArea::Plot].Y -= AreaSizes[EChartContentArea::XAxisBottom].Y;

	FVector2D Offsets[ChartContentAreaCount];

	auto XOffset = 0.0f;

	Offsets[EChartContentArea::YAxisLeftTitle].X = XOffset;
	XOffset += AreaSizes[EChartContentArea::YAxisLeftTitle].X;
	Offsets[EChartContentArea::YAxisLeft].X = XOffset;
	XOffset += AreaSizes[EChartContentArea::YAxisLeft].X;
	
	Offsets[EChartContentArea::Plot].X = XOffset;
	Offsets[EChartContentArea::XAxisBottomTitle].X = XOffset;
	Offsets[EChartContentArea::XAxisTopTitle].X = XOffset;
	Offsets[EChartContentArea::XAxisBottom].X = XOffset;
	Offsets[EChartContentArea::XAxisTop].X = XOffset;
	XOffset += AreaSizes[EChartContentArea::Plot].X;

	Offsets[EChartContentArea::YAxisRight].X = XOffset;
	XOffset += AreaSizes[EChartContentArea::YAxisRight].X;
	Offsets[EChartContentArea::YAxisRightTitle].X = XOffset;
	XOffset += AreaSizes[EChartContentArea::YAxisRightTitle].X;
	
	auto YOffset = 0.0f;

	Offsets[EChartContentArea::XAxisTopTitle].Y = YOffset;
	YOffset += AreaSizes[EChartContentArea::XAxisTopTitle].Y;
	Offsets[EChartContentArea::XAxisTop].Y = YOffset;
	YOffset += AreaSizes[EChartContentArea::XAxisTop].Y;
	
	Offsets[EChartContentArea::Plot].Y = YOffset;
	Offsets[EChartContentArea::YAxisLeftTitle].Y = YOffset;
	Offsets[EChartContentArea::YAxisRightTitle].Y = YOffset;
	Offsets[EChartContentArea::YAxisLeft].Y = YOffset;
	Offsets[EChartContentArea::YAxisRight].Y = YOffset;
	YOffset += AreaSizes[EChartContentArea::Plot].Y;

	Offsets[EChartContentArea::XAxisBottom].Y = YOffset;
	YOffset += AreaSizes[EChartContentArea::XAxisBottom].Y;
	Offsets[EChartContentArea::XAxisBottomTitle].Y = YOffset;
	YOffset += AreaSizes[EChartContentArea::XAxisBottomTitle].Y;
	
	Geoms[EChartContentArea::YAxisLeftTitle] = ContentGeometry.MakeChild(
		FVector2D(AreaSizes[EChartContentArea::YAxisLeftTitle].X, AreaSizes[EChartContentArea::Plot].Y),
		FSlateLayoutTransform(Offsets[EChartContentArea::YAxisLeftTitle])
		);
	Geoms[EChartContentArea::YAxisRightTitle] = ContentGeometry.MakeChild(
		FVector2D(AreaSizes[EChartContentArea::YAxisRightTitle].X, AreaSizes[EChartContentArea::Plot].Y),
		FSlateLayoutTransform(Offsets[EChartContentArea::YAxisRightTitle])
		);
	Geoms[EChartContentArea::YAxisLeft] = ContentGeometry.MakeChild(
		FVector2D(AreaSizes[EChartContentArea::YAxisLeft].X, AreaSizes[EChartContentArea::Plot].Y),
		FSlateLayoutTransform(Offsets[EChartContentArea::YAxisLeft])
		);
	Geoms[EChartContentArea::YAxisRight] = ContentGeometry.MakeChild(
		FVector2D(AreaSizes[EChartContentArea::YAxisRight].X, AreaSizes[EChartContentArea::Plot].Y),
		FSlateLayoutTransform(Offsets[EChartContentArea::YAxisRight])
		);

	Geoms[EChartContentArea::XAxisBottomTitle] = ContentGeometry.MakeChild(
		FVector2D(AreaSizes[EChartContentArea::Plot].X, AreaSizes[EChartContentArea::XAxisBottomTitle].Y),
		FSlateLayoutTransform(Offsets[EChartContentArea::XAxisBottomTitle])
		);
	Geoms[EChartContentArea::XAxisTopTitle] = ContentGeometry.MakeChild(
		FVector2D(AreaSizes[EChartContentArea::Plot].X, AreaSizes[EChartContentArea::XAxisTopTitle].Y),
		FSlateLayoutTransform(Offsets[EChartContentArea::XAxisTopTitle])
		);
	Geoms[EChartContentArea::XAxisBottom] = ContentGeometry.MakeChild(
		FVector2D(AreaSizes[EChartContentArea::Plot].X, AreaSizes[EChartContentArea::XAxisBottom].Y),
		FSlateLayoutTransform(Offsets[EChartContentArea::XAxisBottom])
		);
	Geoms[EChartContentArea::XAxisTop] = ContentGeometry.MakeChild(
		FVector2D(AreaSizes[EChartContentArea::Plot].X, AreaSizes[EChartContentArea::XAxisTop].Y),
		FSlateLayoutTransform(Offsets[EChartContentArea::XAxisTop])
		);

	Geoms[EChartContentArea::Plot] = ContentGeometry.MakeChild(
		AreaSizes[EChartContentArea::Plot],
		FSlateLayoutTransform(Offsets[EChartContentArea::Plot])
		);
}

FVector2D SKantanChart::DetermineTitleAreaSize(FGeometry const& InsetGeometry, bool bIncludePadding) const
{
	if (!HasChartTitle())
	{
		// If empty, don't allocate any size
		return FVector2D(0.0f, 0.0f);
	}

	auto FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

	auto ChartStyle = GetChartStyle();
	auto TitleFont = GetLabelFont(ChartStyle, EKantanChartLabelClass::ChartTitle);
	FVector2D TitleExtent = FontMeasureService->Measure(ChartTitle, TitleFont);

	// @TODO: Configurable title positioning, for now assume top centered
	auto AvailableSize = InsetGeometry.GetLocalSize();
	TitleExtent = FVector2D(
//		FMath::Min(TitleExtent.X + TitlePadding.Left + TitlePadding.Right, AvailableSize.X),
		AvailableSize.X,
		FMath::Min(TitleExtent.Y + TitlePadding.Top + TitlePadding.Bottom, AvailableSize.Y)
		);
	if (!bIncludePadding)
	{
		TitleExtent = FVector2D(
//			FMath::Max(TitleExtent.X - TitlePadding.Left - TitlePadding.Right, 0.0f),
			FMath::Max(AvailableSize.X - TitlePadding.Left - TitlePadding.Right, 0.0f),
			FMath::Max(TitleExtent.Y + TitlePadding.Top + TitlePadding.Bottom, 0.0f)
			);
	}
	return TitleExtent;
}

FText SKantanChart::GetAxisTitleToShow(FCartesianAxisConfig const& AxisCfg, AxisUtil::FAxisMarkerData const& MarkerData) const
{
	auto Title = AxisCfg.Title.IsEmptyOrWhitespace() ? FText::GetEmpty() : AxisCfg.Title;

	if(true)	// @TODO: config
	{
		FText UnitStr = FText::GetEmpty();
		if(AxisCfg.Unit.IsEmptyOrWhitespace() == false)
		{
			UnitStr = FText::Format(FText::FromString(TEXT("{0}")),
				AxisCfg.Unit
			);
		}
		FText ExponentStr = FText::GetEmpty();
		if(MarkerData.DisplayPower != 0)
		{
			ExponentStr = FText::Format(FText::FromString(TEXT("x10^{0}")),
				FText::AsNumber(MarkerData.DisplayPower)
			);
		}
		FText OffsetStr = FText::GetEmpty();
		if(MarkerData.Offset.IsSet())
		{
			OffsetStr = FText::Format(FText::FromString(TEXT("+{0}")),
				FText::AsNumber(MarkerData.Offset.GetValue().GetFloatValue())	// @TODO: display offset in exponent form if above some number of digits
			);
		}
		/*			FText UnitSpace = FText::GetEmpty();
		if (!UnitStr.IsEmpty() && !ExponentStr.IsEmpty())
		{
		UnitSpace = FText::FromString(TEXT(" "));
		}
		*/			
		if(!UnitStr.IsEmpty() || !ExponentStr.IsEmpty() || !OffsetStr.IsEmpty())
		{
			Title = FText::Format(FText::FromString(TEXT("{0} ({1}{2}{3})")),
				Title,
				ExponentStr,
				OffsetStr,
				//UnitSpace,
				UnitStr
			);
		}
	}

	return Title;
}

FVector2D SKantanChart::DetermineAxisTitleSize(FCartesianAxisConfig const& AxisCfg, EAxis::Type AxisOrientation) const
{
	auto ChartStyle = GetChartStyle();
	auto FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	auto LabelFont = GetLabelFont(ChartStyle, EKantanChartLabelClass::AxisTitle);

	auto TitleToUse = LOCTEXT("TitleDummy", "Dummy");
		//GetAxisTitleToShow(AxisCfg, MarkerData);
	// @NOTE: Having to always allocate space even when empty. This is because this method cannot
	// be dependent on plot size, but plot size must be known in order to calculate whether
	// the title will have an offset or display power component.
	auto LabelExtents = /*AxisCfg.Title.IsEmptyOrWhitespace()*/ false ? FVector2D::ZeroVector : FontMeasureService->Measure(TitleToUse, LabelFont);

	FVector2D Size = FVector2D::ZeroVector;
	switch (AxisOrientation)
	{
	case EAxis::X:
		Size = LabelExtents;
		break;

	case EAxis::Y:
		Size.Set(LabelExtents.Y, LabelExtents.X);
		break;

	default:
		check(false);
	}

	return Size;
}

FVector2D SKantanChart::DetermineAxisValueLabelMaxExtents(EAxis::Type Axis, int32 MaxLabelDigits) const
{
	auto FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

	auto ChartStyle = GetChartStyle();
	auto Font = GetLabelFont(ChartStyle, EKantanChartLabelClass::AxisMarkerLabel);
	// Assuming font is fixed width for numeric digits
	FString DummyStr = TEXT("-0.");
	//auto MaxAxisValueDigits = Axis == EAxis::X ? XAxisCfg.MaxValueDigits : YAxisCfg.MaxValueDigits;
	for (int32 Digit = 1; Digit < MaxLabelDigits; ++Digit)
	{
		DummyStr += TEXT("0");
	}
	FText DummyText = FText::FromString(DummyStr);
	return FontMeasureService->Measure(DummyText, Font);
}

float SKantanChart::DetermineAxisRequiredWidth(
	EAxis::Type Axis,
	int32 MaxLabelDigits,
	float AxisMarkerLength,
	float AxisMarkerLabelGap
	) const
{
	float Required = AxisMarkerLength + AxisMarkerLabelGap;

	auto MaxLabelExtents = DetermineAxisValueLabelMaxExtents(Axis, MaxLabelDigits);
	switch (Axis)
	{
	case EAxis::X:
		Required += MaxLabelExtents.Y;
		break;

	case EAxis::Y:
		Required += MaxLabelExtents.X;
		break;
	}

	return Required;
}

bool SKantanChart::WillGeometryGetClipped(FGeometry const& Geom, FSlateRect const& ClipRect)
{
	auto AbsTopLeft = Geom.LocalToAbsolute(FVector2D(0, 0));
	auto AbsBottomRight = Geom.LocalToAbsolute(Geom.GetLocalSize());
	FSlateRect AbsTextRect{ AbsTopLeft, AbsBottomRight };
	return FSlateRect::IsRectangleContained(ClipRect, AbsTextRect) == false;
}

FGeometry SKantanChart::MakeTitleGeometry(FGeometry const& InsetGeometry) const
{
	auto AvailableSize = InsetGeometry.GetLocalSize();
	auto TitleExtent = DetermineTitleAreaSize(InsetGeometry, false);
	return InsetGeometry.MakeChild(
		FVector2D(TitleExtent.X, TitleExtent.Y),
		FSlateLayoutTransform(FVector2D(TitlePadding.Left, TitlePadding.Top))
		);
}

FGeometry SKantanChart::MakeContentGeometry(FGeometry const& InsetGeometry) const
{
	auto AvailableSize = InsetGeometry.GetLocalSize();
	auto TitleExtent = DetermineTitleAreaSize(InsetGeometry, true);
	return InsetGeometry.MakeChild(
		FVector2D(AvailableSize.X, AvailableSize.Y - TitleExtent.Y),
		FSlateLayoutTransform(FVector2D(0.0f, TitleExtent.Y))
		);
}

int32 SKantanChart::DrawChartBackground(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FKantanChartStyle* Style, const FWidgetStyle& InWidgetStyle) const
{
	// Default to transparent brush
	auto FallbackBrush = FSlateColorBrush(FLinearColor(0, 0, 0, 0));
	auto BackgroundBrush = Style ? &Style->Background : &FallbackBrush;
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId++,
		AllottedGeometry.ToPaintGeometry(),
		BackgroundBrush,
		//MyClippingRect,
		ESlateDrawEffect::None,
		BackgroundBrush->GetTint(InWidgetStyle)
		);

	return LayerId;
}

int32 SKantanChart::DrawChartTitle(const FPaintArgs& Args, const FGeometry& TitleGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FKantanChartStyle* Style, const FWidgetStyle& InWidgetStyle) const
{
	auto ChartStyle = GetChartStyle();

	auto FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	auto TitleFont = GetLabelFont(ChartStyle, EKantanChartLabelClass::ChartTitle);
	FVector2D TitleExtent = FontMeasureService->Measure(ChartTitle, TitleFont);

	auto TextGeometry = TitleGeometry.MakeChild(
		TitleExtent,
		FSlateLayoutTransform(FVector2D((TitleGeometry.GetLocalSize().X - TitleExtent.X) * 0.5f, 0.0f))
		);
	FSlateDrawElement::MakeText(
		OutDrawElements,
		LayerId++,
		TextGeometry.ToPaintGeometry(),
		ChartTitle,
		TitleFont,
		//MyClippingRect,
		ESlateDrawEffect::None,
		ChartStyle->FontColor);

	return LayerId;
}

int32 SKantanChart::DrawFixedAxis(
	const FGeometry& Geometry,
	const FSlateRect& ClipRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	EAxis::Type Axis,
	AxisUtil::FAxisTransform const& ValueToChartAxisTransform,
	EChartAxisPosition AxisPosition,
	AxisUtil::FAxisMarkerData const& MarkerData,
	bool bDrawMarkers,
	bool bDrawLabels,
	float AxisMarkerLength,
	float AxisMarkerLabelGap
	) const
{
	const int32 AxisIdx = Axis == EAxis::X ? 0 : 1;
	const int32 OtherAxisIdx = Axis == EAxis::X ? 1 : 0;

	auto ChartStyle = GetChartStyle();
	auto ValueFont = GetLabelFont(ChartStyle, EKantanChartLabelClass::AxisMarkerLabel);
	auto FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

	auto AxisStart = 0.0f;
	auto AxisEnd = Geometry.GetLocalSize()[AxisIdx];
	auto CartesianRange = FCartesianAxisRange(
		ValueToChartAxisTransform.Inverse().MapPoint(AxisStart),
		ValueToChartAxisTransform.Inverse().MapPoint(AxisEnd)
		);
	CartesianRange = CartesianRange.Normalized();

	// Flag denoting if perpendicular coordinate in geometry space is reversed (ie. going away from plot area is negative)
	auto const bIsReversed =
		(AxisPosition == EChartAxisPosition::RightTop && Axis == EAxis::X) ||
		(AxisPosition == EChartAxisPosition::LeftBottom && Axis == EAxis::Y);
	auto const FixedCompBase = bIsReversed ? Geometry.GetLocalSize()[OtherAxisIdx] : 0.0f;

	{
		// Draw the axis line
		TArray< FVector2D > Points;
		Points.Init(FVector2D::ZeroVector, 2);
		Points[0][AxisIdx] = 0.0f;
		Points[0][OtherAxisIdx] = FixedCompBase;
		Points[1][AxisIdx] = Geometry.GetLocalSize()[AxisIdx];
		Points[1][OtherAxisIdx] = FixedCompBase;

		FSlateDrawElement::MakeLines(
			OutDrawElements,
			LayerId,
			Geometry.ToPaintGeometry(),
			Points,
			//ClipRect,
			ESlateDrawEffect::None,
			ChartStyle->ChartLineColor,
			false,
			ChartStyle->ChartLineThickness
			);
	}


	auto const& Rounding = MarkerData.RL;

	/*	if (false)	// @TODO: if show axis multiplier and unit on axis
	{
	// Axis unit text
	FText UnitText = FText::Format(
	FText::FromString(TEXT("x{0}")),
	FText::FromString(XRounding.ExponentAsString())
	);
	auto UnitExtents = FontMeasureService->Measure(UnitText, AxisMarkerFont);
	auto UnitGeometry = PlotSpaceGeometry.MakeChild(
	UnitExtents,
	FSlateLayoutTransform(FVector2D(PlotSize.X - UnitExtents.X, Y0 - UnitExtents.Y))
	);
	FSlateDrawElement::MakeText(
	OutDrawElements,
	LayerId,
	UnitGeometry.ToPaintGeometry(),
	UnitText,
	AxisMarkerFont,
	ClipRect,
	ESlateDrawEffect::None,
	ChartStyle->FontColor);
	}
	*/

	auto const MarkerOffset = bIsReversed ? -AxisMarkerLength : AxisMarkerLength;
	auto const MarkerLabelGapOffset = bIsReversed ? -AxisMarkerLabelGap : AxisMarkerLabelGap;

	// Axis markers and labels
	for(auto const& AbsoluteRoundedMarker : MarkerData.MarkerValues)
	{
		auto RoundedMarker = MarkerData.Offset.IsSet() ?
			AbsoluteRoundedMarker.RelativeTo(MarkerData.Offset.GetValue()) :
			AbsoluteRoundedMarker;

		auto AbsMarkerVal = AbsoluteRoundedMarker.GetFloatValue();
		auto MarkerPlotSpace = ValueToChartAxisTransform.MapPoint(AbsMarkerVal);
		TArray< FVector2D > Points;
		Points.Init(FVector2D::ZeroVector, 2);
		Points[0][AxisIdx] = MarkerPlotSpace;
		Points[0][OtherAxisIdx] = FixedCompBase;
		Points[1][AxisIdx] = MarkerPlotSpace;
		Points[1][OtherAxisIdx] = FixedCompBase + MarkerOffset;

		if (bDrawMarkers)
		{
			FSlateDrawElement::MakeLines(
				OutDrawElements,
				LayerId,
				Geometry.ToPaintGeometry(),
				Points,
				//ClipRect,
				ESlateDrawEffect::None,
				ChartStyle->ChartLineColor,
				false,
				ChartStyle->ChartLineThickness
			);
		}

		if (bDrawLabels)
		{
			FText UnsignedLabelText = FText::FromString(RoundedMarker.Abs().MultiplierAsString(MarkerData.DisplayPower));
			auto UnsignedLabelExtents = FontMeasureService->Measure(UnsignedLabelText, ValueFont);
			FText LabelText = FText::FromString(RoundedMarker.MultiplierAsString(MarkerData.DisplayPower));
			auto LabelExtents = FontMeasureService->Measure(LabelText, ValueFont);

			// Offset label perpendicularly to the axis based on which side of the plot the axis is on
			auto LabelOffset = FVector2D::ZeroVector;
			LabelOffset[OtherAxisIdx] += MarkerLabelGapOffset;
			if (bIsReversed)
			{
				LabelOffset[OtherAxisIdx] -= LabelExtents[OtherAxisIdx];
			}
			// Center the label on the marker
			LabelOffset[AxisIdx] -= LabelExtents[AxisIdx] * 0.5f;
			if (Axis == EAxis::X)
			{
				// Adjust centering to ignore '-' sign
				LabelOffset[AxisIdx] -= (LabelExtents[AxisIdx] - UnsignedLabelExtents[AxisIdx]) * 0.5f;
			}

			auto LabelGeometry = Geometry.MakeChild(
				LabelExtents,
				FSlateLayoutTransform(Points[1] + LabelOffset)
				);
			// Test if the text can be fully displayed within the clip bounds, and if not, don't draw it at all
			if(!WillGeometryGetClipped(LabelGeometry, ClipRect))
			{
				FSlateDrawElement::MakeText(
					OutDrawElements,
					LayerId,
					LabelGeometry.ToPaintGeometry(),
					LabelText,
					ValueFont,
					//ClipRect,
					ESlateDrawEffect::None,
					ChartStyle->FontColor);
			}
		}
	}

	return LayerId;
}

int32 SKantanChart::DrawXAxisTitle(const FGeometry& Geometry, const FSlateRect& ClipRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, FCartesianAxisConfig const& AxisCfg, AxisUtil::FAxisMarkerData const& MarkerData) const
{
	auto FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	auto ChartStyle = GetChartStyle();
	auto LabelFont = GetLabelFont(ChartStyle, EKantanChartLabelClass::AxisTitle);

	auto ActualTitle = GetAxisTitleToShow(AxisCfg, MarkerData);
	if (ActualTitle.IsEmptyOrWhitespace() == false)
	{
		auto Label = ActualTitle;
		auto Extents = FontMeasureService->Measure(Label, LabelFont);
		auto AvailableSize = Geometry.GetLocalSize();
		FSlateDrawElement::MakeText(
			OutDrawElements,
			LayerId,
			Geometry.ToPaintGeometry(Extents, FSlateLayoutTransform(FVector2D((AvailableSize.X - Extents.X) * 0.5f, 0.0f))),
			Label,
			LabelFont,
			//ClipRect,
			ESlateDrawEffect::None,
			ChartStyle->FontColor);
	}

	return LayerId;
}

int32 SKantanChart::DrawYAxisTitle(const FGeometry& Geometry, const FSlateRect& ClipRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, FCartesianAxisConfig const& AxisCfg, AxisUtil::FAxisMarkerData const& MarkerData) const
{
	auto FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	auto ChartStyle = GetChartStyle();
	auto LabelFont = GetLabelFont(ChartStyle, EKantanChartLabelClass::AxisTitle);

	auto ActualTitle = GetAxisTitleToShow(AxisCfg, MarkerData);
	if(ActualTitle.IsEmptyOrWhitespace() == false)
	{
		auto Label = ActualTitle;
		auto Extents = FontMeasureService->Measure(Label, LabelFont);
		auto AvailableSize = Geometry.GetLocalSize();

		auto const RenderXform = TransformCast< FSlateRenderTransform >(Concatenate(Inverse(FVector2D(Extents.X, 0.0f)), FQuat2D(FMath::DegreesToRadians(-90.0f))));
		auto RotatedGeometry = Geometry.MakeChild(
			Extents,
			FSlateLayoutTransform(FVector2D(0.0f, (AvailableSize.Y - Extents.X) * 0.5f)),
			RenderXform,
			FVector2D(0.0f, 0.0f)
			);
		FSlateRect BaseClipRect(
			RotatedGeometry.GetAccumulatedLayoutTransform().TransformPoint(FVector2D(0, 0)),
			RotatedGeometry.GetAccumulatedLayoutTransform().TransformPoint(FVector2D(Extents.Y, Extents.X))
			);
		auto IntersectedClipRect = BaseClipRect.IntersectionWith(ClipRect);
		auto TransformedClipRect = TransformRect(
			Concatenate(
			Inverse(RotatedGeometry.GetAccumulatedLayoutTransform()),
			Inverse(RenderXform),
			RotatedGeometry.GetAccumulatedLayoutTransform()
			),
			FSlateRotatedRect(IntersectedClipRect)
			).ToBoundingRect();
		auto FinalClipRect = TransformedClipRect;
		FSlateDrawElement::MakeText(
			OutDrawElements,
			LayerId,
			RotatedGeometry.ToPaintGeometry(),
			Label,
			LabelFont,
			//FinalClipRect,
			ESlateDrawEffect::None,
			ChartStyle->FontColor);
	}

	return LayerId;
}

void SKantanChart::SetActiveTimerTickRate(float TickRate)
{
	if (UpdateTickRate != TickRate || !ActiveTimerHandle.IsValid())
	{
		UpdateTickRate = TickRate;

		TSharedPtr<FActiveTimerHandle> SharedActiveTimerHandle = ActiveTimerHandle.Pin();
		if (SharedActiveTimerHandle.IsValid())
		{
			UnRegisterActiveTimer(SharedActiveTimerHandle.ToSharedRef());
		}

		ActiveTimerHandle = RegisterActiveTimer(
			UpdateTickRate,
			FWidgetActiveTimerDelegate::CreateSP(this, &SKantanChart::ActiveTick)
			);
	}
}

EActiveTimerReturnType SKantanChart::ActiveTick(double InCurrentTime, float InDeltaTime)
{
	OnActiveTick(InCurrentTime, InDeltaTime);
	return EActiveTimerReturnType::Continue;
}


int32 SKantanChart::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	// Pre-snap the clipping rect to try and reduce common jitter, since the padding is typically only a single pixel.
	FSlateRect SnappedClippingRect = FSlateRect(FMath::RoundToInt(MyClippingRect.Left), FMath::RoundToInt(MyClippingRect.Top), FMath::RoundToInt(MyClippingRect.Right), FMath::RoundToInt(MyClippingRect.Bottom));

	auto ChartStyle = GetChartStyle();
	LayerId = DrawChartBackground(
		Args,
		AllottedGeometry,
		SnappedClippingRect,
		OutDrawElements,
		LayerId,
		ChartStyle,
		InWidgetStyle);

	auto InsetGeometry = MakeInsetGeometry(AllottedGeometry);

	if (HasChartTitle())
	{
		auto TitleGeometry = MakeTitleGeometry(InsetGeometry);
		// @TODO: Using deprecated GetClippingRect which ignores render transforms.
		// Not sure of better way to do this though.
		auto ClipRect = SnappedClippingRect;// TitleGeometry.GetClippingRect().IntersectionWith(SnappedClippingRect);
		LayerId = DrawChartTitle(Args, TitleGeometry, ClipRect, OutDrawElements, LayerId, ChartStyle, InWidgetStyle);
	}

	{
		auto ContentGeometry = MakeContentGeometry(InsetGeometry);
		FGeometry AreaGeoms[ChartContentAreaCount];
		MakeAreaGeometries(ContentGeometry, AreaGeoms);
		// @TODO: Using deprecated GetClippingRect which ignores render transforms.
		// Not sure of better way to do this though.
		auto ClipRect = SnappedClippingRect;
		//InsetGeometry.GetLayoutBoundingRect();
			//SnappedClippingRect;// ContentGeometry.GetClippingRect().IntersectionWith(SnappedClippingRect);
		for (int32 AreaIdx = 0; AreaIdx < ChartContentAreaCount; ++AreaIdx)
		{
			if (AreaGeoms[AreaIdx].GetLocalSize().X > 0.0f &&
				AreaGeoms[AreaIdx].GetLocalSize().Y > 0.0f)
			{
				LayerId = DrawChartArea(
					(EChartContentArea::Type)AreaIdx,
					Args,
					AreaGeoms[AreaIdx],
					AreaGeoms[EChartContentArea::Plot],
					ClipRect,
					OutDrawElements,
					LayerId,
					InWidgetStyle,
					bParentEnabled
					);
			}
		}
	}

	return LayerId;
}

FReply SKantanChart::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if(OnChartMouseDown.IsBound())
	{
		// @TODO: Area detection
		OnChartMouseDown.Execute(KantanCharts::EChartInteractionArea::Plot, MouseEvent);
	}

	return SLeafWidget::OnMouseButtonDown(MyGeometry, MouseEvent);
}


#undef LOCTEXT_NAMESPACE

