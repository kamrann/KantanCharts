// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "Charts/SKantanBarChart.h"
#include "KantanCartesianTypes.h"
#include "ChartConstants.h"

#include "Framework/Application/SlateApplication.h"
#include "Fonts/FontMeasure.h"


void SKantanBarChart::Construct(const FArguments& InArgs)
{
	SKantanCategoryChart::Construct(
		SKantanCategoryChart::FArguments()
		.Datasource(InArgs._Datasource)
		.UpdateTickRate(InArgs._UpdateTickRate)
		);

	SetStyle(InArgs._Style);
	SetOrientation(InArgs._Orientation);
	SetMaxBarValue(InArgs._MaxBarValue);
	SetLabelPosition(InArgs._LabelPosition);
	SetBarToGapRatio(InArgs._BarToGapRatio);
	SetDrawCategoryBoundaries(InArgs._DrawCategoryBoundaries);
	SetValueExtentsDisplay(InArgs._ValueExtentsDisplay);

	BarInteriorBrush = MakeShareable< FSlateBrush >(new FSlateColorBrush(FColor::White));
	BarBorderBrush = MakeShareable< FSlateBrush >(new FSlateBorderBrush(NAME_None, FMargin(1.0f), FLinearColor::White, ESlateBrushImageType::NoImage));
}

void SKantanBarChart::SetStyle(const FKantanBarChartStyle* InStyle)
{
	Style = InStyle;
}

void SKantanBarChart::SetStyleFromAsset(USlateWidgetStyleContainerBase* InStyleAsset)
{
	if(auto Asset = Cast< UKantanBarChartWidgetStyle >(InStyleAsset))
	{
		Style = &Asset->ChartStyle;
	}
}

void SKantanBarChart::SetOrientation(EKantanBarChartOrientation InOrientation)
{
	Orientation = InOrientation;
}

void SKantanBarChart::SetMaxBarValue(float InMax)
{
	MaxBarValue = InMax;
}

void SKantanBarChart::SetLabelPosition(EKantanBarLabelPosition InPosition)
{
	LabelPosition = InPosition;
}

void SKantanBarChart::SetBarToGapRatio(float InRatio)
{
	BarToGapRatio = InRatio;
}

void SKantanBarChart::SetDrawCategoryBoundaries(bool InDraw)
{
	bDrawCategoryBoundaries = InDraw;
}

void SKantanBarChart::SetValueExtentsDisplay(EKantanBarValueExtents InExtents)
{
	ValueExtentsDisplay = InExtents;
}

void SKantanBarChart::SetValueAxisConfig(FCartesianAxisConfig const& InConfig)
{
	ValueAxisCfg = InConfig;
}

FCartesianAxisRange SKantanBarChart::DetermineValueRange() const
{
	FCartesianAxisRange Range;
	Range.Min = 0.0f;
	
	if (MaxBarValue == 0.0f)
	{
		// Auto normalize to highest
		auto NumBars = GetNumCategories();
		float EffectiveMaxBarValue = MaxBarValue;
		for (int32 Idx = 0; Idx < NumBars; ++Idx)
		{
			auto Value = GetCategoryValue(Idx);
			EffectiveMaxBarValue = FMath::Max(EffectiveMaxBarValue, Value);
		}
		Range.Max = EffectiveMaxBarValue;
	}
	else
	{
		Range.Max = MaxBarValue;
	}

	// Avoid issues arising from a zero size range
	if (Range.Size() == 0.0f)
	{
		Range.Max = Range.Min + 1.0f;
	}

	return Range;
}

AxisUtil::FAxisTransform SKantanBarChart::DetermineValueAxisTransform(FVector2D const& PlotSize) const
{
	auto ValueRange = DetermineValueRange();

	// @TODO: This code just uses the X coordinate only, regardless of which axis we are dealing with.
	// Really shouldn't implement FAxisTransform in terms of a 2D transform.
	FTransform2D Xform;
	switch (Orientation)
	{
	case EKantanBarChartOrientation::Vertical:
		Xform = Concatenate(
			FTransform2D(FVector2D(-ValueRange.Min, 0.0f)),
			FTransform2D(FScale2D(-PlotSize.Y / ValueRange.Size(), 1.0f)),
			FTransform2D(FVector2D(PlotSize.Y, 0.0f))
			);
		break;
	case EKantanBarChartOrientation::Horizontal:
		Xform = Concatenate(
			FTransform2D(FVector2D(-ValueRange.Min, 0.0f)),
			FTransform2D(FScale2D(PlotSize.X / ValueRange.Size(), 1.0f)),
			FTransform2D(FVector2D(0.0f, 0.0f))
			);
		break;
	}

	return AxisUtil::FAxisTransform::FromTransform2D(Xform, 0);
}

void SKantanBarChart::DetermineCategoryLabelRequirements(float InBarWidth, float& ReqSize, float& Rotation) const
{
	auto ChartStyle = GetChartStyle();
	auto FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	FSlateFontInfo FontInfo = GetLabelFont(ChartStyle, EKantanChartLabelClass::AxisTitle);
	auto NumBars = GetNumCategories();

	float RequiredLabelSize = 0.0f;
	FVector2D LabelMaxExtents = FVector2D::ZeroVector;
	for (int32 Idx = 0; Idx < NumBars; ++Idx)
	{
		if (LabelPosition == EKantanBarLabelPosition::Standard)
		{
			auto Label = GetCategoryLabel(Idx);
			FVector2D LabelExtent = FontMeasureService->Measure(Label, FontInfo);
			LabelMaxExtents = FVector2D(
				FMath::Max(LabelMaxExtents.X, LabelExtent.X),
				FMath::Max(LabelMaxExtents.Y, LabelExtent.Y)
				);
		}
	}

	float LabelRotation = 0.0f;	// Radians
	if (LabelPosition == EKantanBarLabelPosition::Standard)
	{
		switch (Orientation)
		{
		case EKantanBarChartOrientation::Horizontal:
		{
			RequiredLabelSize = LabelMaxExtents.X;
		}
		break;

		case EKantanBarChartOrientation::Vertical:
		{
			auto const AvailableWidth = InBarWidth;
			if (AvailableWidth >= LabelMaxExtents.X)
			{
				// No need to rotate labels
				RequiredLabelSize = LabelMaxExtents.Y;
			}
			else
			{
				auto const Diagonal = LabelMaxExtents.Size();
				auto const Theta = FMath::Acos(AvailableWidth / Diagonal);
				auto const Alpha = FMath::Atan(LabelMaxExtents.Y / LabelMaxExtents.X);
				if (Alpha + Theta >= PI / 2)
				{
					// Need full rotation to vertical (but don't rotate beyond)
					LabelRotation = PI / 2;
					RequiredLabelSize = LabelMaxExtents.X;
				}
				else
				{
					// Rotate as much as necessary to fit into available width
					LabelRotation = Alpha + Theta;
					RequiredLabelSize = Diagonal * FMath::Sin(2 * Alpha + Theta);
				}
			}
		}
		break;
		}
	}

	// Add on the desired gap between axis line and category labels.
	ReqSize = RequiredLabelSize + ChartConstants::AxisCategoryLabelGap;
	Rotation = LabelRotation;
}

float SKantanBarChart::GetChartAreaSize(EChartContentArea::Type Area, EAxis::Type ReqComp, FVector2D const& KnownPlotSize) const
{
	switch (Area)
	{
	case EChartContentArea::YAxisLeftTitle:
		if (ReqComp == EAxis::X)
		{
			if (Orientation == EKantanBarChartOrientation::Vertical && ValueAxisCfg.LeftBottomAxis.bEnabled && ValueAxisCfg.LeftBottomAxis.bShowTitle)
			{
				return DetermineAxisTitleSize(ValueAxisCfg, EAxis::Y).X;
			}
		}
		break;
	case EChartContentArea::YAxisRightTitle:
		if (ReqComp == EAxis::X)
		{
			if (Orientation == EKantanBarChartOrientation::Vertical && ValueAxisCfg.RightTopAxis.bEnabled && ValueAxisCfg.RightTopAxis.bShowTitle)
			{
				return DetermineAxisTitleSize(ValueAxisCfg, EAxis::Y).X;
			}
		}
		break;
	case EChartContentArea::XAxisBottomTitle:
		if (ReqComp == EAxis::Y)
		{
			if (Orientation == EKantanBarChartOrientation::Horizontal && ValueAxisCfg.LeftBottomAxis.bEnabled && ValueAxisCfg.LeftBottomAxis.bShowTitle)
			{
				return DetermineAxisTitleSize(ValueAxisCfg, EAxis::X).Y;
			}
		}
		break;
	case EChartContentArea::XAxisTopTitle:
		if (ReqComp == EAxis::Y)
		{
			if (Orientation == EKantanBarChartOrientation::Horizontal && ValueAxisCfg.RightTopAxis.bEnabled && ValueAxisCfg.RightTopAxis.bShowTitle)
			{
				return DetermineAxisTitleSize(ValueAxisCfg, EAxis::X).Y;
			}
		}
		break;

	case EChartContentArea::YAxisLeft:
		if (ReqComp == EAxis::X)
		{
			if (Orientation == EKantanBarChartOrientation::Vertical && ValueAxisCfg.LeftBottomAxis.bEnabled)
			{
				return DetermineAxisRequiredWidth(EAxis::Y, ValueAxisCfg.MaxValueDigits, ChartConstants::AxisMarkerLength, ChartConstants::AxisMarkerLabelGap);
			}
			else if (Orientation == EKantanBarChartOrientation::Horizontal)
			{
				float ReqSize, Rotation;
				/* Passing 0 for BarWidth, since we can't work it out (X sizes are computed before Y sizes,
				so KnownPlotSize will be zero), and it is not needed anyway since in the horizontal orientation,
				text does not get rotated. */
				DetermineCategoryLabelRequirements(0.0f, ReqSize, Rotation);
				return ReqSize;
			}
		}
		break;
	case EChartContentArea::YAxisRight:
		if (ReqComp == EAxis::X)
		{
			if (Orientation == EKantanBarChartOrientation::Vertical && ValueAxisCfg.RightTopAxis.bEnabled)
			{
				return DetermineAxisRequiredWidth(EAxis::Y, ValueAxisCfg.MaxValueDigits, ChartConstants::AxisMarkerLength, ChartConstants::AxisMarkerLabelGap);
			}
		}
		break;
	case EChartContentArea::XAxisBottom:
		if (ReqComp == EAxis::Y)
		{
			if (Orientation == EKantanBarChartOrientation::Horizontal && ValueAxisCfg.LeftBottomAxis.bEnabled)
			{
				return DetermineAxisRequiredWidth(EAxis::X, ValueAxisCfg.MaxValueDigits, ChartConstants::AxisMarkerLength, ChartConstants::AxisMarkerLabelGap);
			}
			else if (Orientation == EKantanBarChartOrientation::Vertical)
			{
				float ReqSize, Rotation;
				DetermineCategoryLabelRequirements(CalcBarWidth(GetNumCategories(), KnownPlotSize.X), ReqSize, Rotation);
				return ReqSize;
			}
		}
		break;
	case EChartContentArea::XAxisTop:
		if (ReqComp == EAxis::Y)
		{
			if (Orientation == EKantanBarChartOrientation::Horizontal && ValueAxisCfg.RightTopAxis.bEnabled)
			{
				return DetermineAxisRequiredWidth(EAxis::X, ValueAxisCfg.MaxValueDigits, ChartConstants::AxisMarkerLength, ChartConstants::AxisMarkerLabelGap);
			}
		}
		break;
	}

	return 0.0f;
}

FFloatRoundingLevel SKantanBarChart::DetermineValueAxisRoundingLevel(FGeometry const& PlotSpaceGeometry) const
{
	auto const AxisTransform = DetermineValueAxisTransform(PlotSpaceGeometry.GetLocalSize());

	auto const DefaultMinPlotSpaceLabelSeparation = 50.0f;
	auto MinPlotSpaceLabelSeparation = DefaultMinPlotSpaceLabelSeparation * ValueAxisCfg.MarkerSpacing;
	MinPlotSpaceLabelSeparation = FMath::Max(MinPlotSpaceLabelSeparation, 1.0f);

	return AxisUtil::DetermineAxisRoundingLevel(AxisTransform, MinPlotSpaceLabelSeparation);
}

AxisUtil::FAxisMarkerData SKantanBarChart::DetermineValueAxisMarkerData(FGeometry const& PlotSpaceGeometry) const
{
	auto ValueRange = DetermineValueRange();

	return AxisUtil::DetermineAxisMarkerData(
		DetermineValueAxisRoundingLevel(PlotSpaceGeometry),
		ValueRange,
		ValueAxisCfg.MaxValueDigits
		);
}

void SKantanBarChart::DrawCategoryAxis(
	EChartContentArea::Type Area,
	const FPaintArgs& Args,
	const FGeometry& Geometry,
	const FGeometry& PlotSpaceGeometry,
	const FSlateRect& MyClippingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled
	) const
{
	auto ChartStyle = GetChartStyle();
	auto FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	FSlateFontInfo FontInfo = GetLabelFont(ChartStyle, EKantanChartLabelClass::AxisTitle);

	auto const OverlaidLabelOffset = 5.0f;	// @TODO: Perhaps proportional amount?

	auto NumBars = GetNumCategories();

	auto const BarWidthProportion = CalcBarWidthProportion(NumBars);
	auto const GapWidthProportion = BarWidthProportion / BarToGapRatio;

	int32 ChartXComp = Orientation == EKantanBarChartOrientation::Vertical ? 0 : 1;
	int32 ChartYComp = Orientation == EKantanBarChartOrientation::Vertical ? 1 : 0;

	auto AvailableSize = PlotSpaceGeometry.GetLocalSize();
	auto const BarWidth = AvailableSize[ChartXComp] * BarWidthProportion;
	auto const GapWidth = AvailableSize[ChartXComp] * GapWidthProportion;
	
	float RequiredLabelSize, LabelRotation;
	DetermineCategoryLabelRequirements(BarWidth, RequiredLabelSize, LabelRotation);

	/*// Debug drawing
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		RetLayerId,
		Geometry.ToPaintGeometry(),
		BarBorderBrush.Get(),
		MyClippingRect,
		ESlateDrawEffect::None,
		FLinearColor::Red
		);
	/*/

	for (int32 Idx = 0; Idx < NumBars; ++Idx)
	{
		if (LabelPosition != EKantanBarLabelPosition::NoLabels)
		{
			auto Label = GetCategoryLabel(Idx);
			FVector2D BaseLabelExtent = FontMeasureService->Measure(Label, FontInfo);
			FVector2D RotatedLabelExtent = FVector2D(
				BaseLabelExtent.X * FMath::Cos(LabelRotation) + BaseLabelExtent.Y * FMath::Sin(LabelRotation),
				BaseLabelExtent.X * FMath::Sin(LabelRotation) + BaseLabelExtent.Y * FMath::Cos(LabelRotation)
				);
			auto LabelBase = 0.0f;
			switch (LabelPosition)
			{
			case EKantanBarLabelPosition::Standard:
				LabelBase = BaseLabelExtent.X * FMath::Sin(LabelRotation);
				break;

			case EKantanBarLabelPosition::Overlaid:
				LabelBase = OverlaidLabelOffset;
				break;
			}

			FVector2D LabelTranslation = FVector2D::ZeroVector;
			LabelTranslation[ChartXComp] = BarWidth * Idx + GapWidth * (0.5f + Idx) + (BarWidth - RotatedLabelExtent[ChartXComp]) * 0.5f;
			LabelTranslation[ChartYComp] = Orientation == EKantanBarChartOrientation::Vertical ?
				(LabelBase + ChartConstants::AxisCategoryLabelGap) :
				0.0f;
			auto RotationTransform = FSlateRenderTransform(FQuat2D(-LabelRotation));

			auto LabelGeom = Geometry.MakeChild(
				BaseLabelExtent,
				FSlateLayoutTransform(LabelTranslation),
				RotationTransform,
				FVector2D(0.0f, 0.0f)
				);

			/*// Debug drawing
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				RetLayerId,
				LabelGeom.ToPaintGeometry(),
				BarBorderBrush.Get(),
				FSlateRect(0, 0, 2000, 2000),
				ESlateDrawEffect::None,
				FLinearColor::Red
				);
			/*/

			//FSlateRect LabelClipRect(
			//	LabelGeom.GetAccumulatedLayoutTransform().TransformPoint(FVector2D(0.0f, 0.0f)),
			//	LabelGeom.GetAccumulatedLayoutTransform().TransformPoint(LabelGeom.GetLocalSize())
			//	);

			FSlateDrawElement::MakeText(
				OutDrawElements,
				LayerId,
				LabelGeom.ToPaintGeometry(),
				Label,
				FontInfo,
				//ChartConstants::RotatedTextClipRect,
				ESlateDrawEffect::None,
				ChartStyle->FontColor);

			if (ValueExtentsDisplay != EKantanBarValueExtents::NoValueLines)
			{
				if (bDrawCategoryBoundaries)
				{
//					for (int32 Idx = 0; Idx < NumBars; ++Idx)
					{
						// @TODO: This is an attempt to align the marker flush with the edge of the bar, but not working consistently
						auto const TruncationOffset = 1.0f;

						TArray< FVector2D > Points;
						Points.Add(MapChartToLocal(FVector2D(BarWidth * Idx + GapWidth * (0.5f + Idx), 0.0f), FVector2D::ZeroVector, AvailableSize));
						Points.Add(MapChartToLocal(FVector2D(BarWidth * Idx + GapWidth * (0.5f + Idx), 0.0f), FVector2D::ZeroVector, AvailableSize));
						Points[0][ChartYComp] = Orientation == EKantanBarChartOrientation::Vertical ? 0.0f : Geometry.GetLocalSize().X;
						//Points[0][ChartXComp] = FMath::TruncToFloat(Points[0][ChartXComp] + TruncationOffset);
						Points[1][ChartYComp] = Orientation == EKantanBarChartOrientation::Vertical ? ChartConstants::AxisMarkerLength : (Geometry.GetLocalSize().X - ChartConstants::AxisMarkerLength);
						//Points[1][ChartXComp] = FMath::TruncToFloat(Points[1][ChartXComp] + TruncationOffset);

						FSlateDrawElement::MakeLines(
							OutDrawElements,
							LayerId,
							Geometry.ToPaintGeometry(),
							Points,
							//MyClippingRect,
							ESlateDrawEffect::None,
							ChartStyle->ChartLineColor,
							false,
							ChartStyle->ChartLineThickness
						);

						Points.Empty(2);
						Points.Add(MapChartToLocal(FVector2D(BarWidth * (Idx + 1) + GapWidth * (0.5f + Idx), 0.0f), FVector2D::ZeroVector, AvailableSize));
						Points.Add(MapChartToLocal(FVector2D(BarWidth * (Idx + 1) + GapWidth * (0.5f + Idx), 0.0f), FVector2D::ZeroVector, AvailableSize));
						Points[0][ChartYComp] = Orientation == EKantanBarChartOrientation::Vertical ? 0.0f : Geometry.GetLocalSize().X;
						Points[1][ChartYComp] = Orientation == EKantanBarChartOrientation::Vertical ? ChartConstants::AxisMarkerLength : (Geometry.GetLocalSize().X - ChartConstants::AxisMarkerLength);

						FSlateDrawElement::MakeLines(
							OutDrawElements,
							LayerId,
							Geometry.ToPaintGeometry(),
							Points,
							//MyClippingRect,
							ESlateDrawEffect::None,
							ChartStyle->ChartLineColor,
							false,
							ChartStyle->ChartLineThickness
						);
					}
				}
			}
		}
	}
}

int32 SKantanBarChart::DrawChartArea(
	EChartContentArea::Type Area,
	const FPaintArgs& Args,
	const FGeometry& Geometry,
	const FGeometry& PlotSpaceGeometry,
	const FSlateRect& MyClippingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled
	) const
{
	bool bEnabled = ShouldBeEnabled(bParentEnabled);
	const ESlateDrawEffect DrawEffects = bEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

	// Paint inside the border only. 
	// Pre-snap the clipping rect to try and reduce common jitter, since the padding is typically only a single pixel.
	FSlateRect SnappedClippingRect = FSlateRect(FMath::RoundToInt(MyClippingRect.Left), FMath::RoundToInt(MyClippingRect.Top), FMath::RoundToInt(MyClippingRect.Right), FMath::RoundToInt(MyClippingRect.Bottom));

	auto ChartStyle = GetChartStyle();

	switch (Area)
	{
	case EChartContentArea::XAxisBottomTitle:
		if (Orientation == EKantanBarChartOrientation::Horizontal && ValueAxisCfg.LeftBottomAxis.bEnabled && ValueAxisCfg.LeftBottomAxis.bShowTitle)
		{
			DrawXAxisTitle(Geometry, SnappedClippingRect, OutDrawElements, LayerId + BarChartLayers::ValueAxis, ValueAxisCfg, DetermineValueAxisMarkerData(PlotSpaceGeometry));
		}
		break;
	case EChartContentArea::XAxisTopTitle:
		if (Orientation == EKantanBarChartOrientation::Horizontal && ValueAxisCfg.RightTopAxis.bEnabled && ValueAxisCfg.RightTopAxis.bShowTitle)
		{
			DrawXAxisTitle(Geometry, SnappedClippingRect, OutDrawElements, LayerId + BarChartLayers::ValueAxis, ValueAxisCfg, DetermineValueAxisMarkerData(PlotSpaceGeometry));
		}
		break;
	case EChartContentArea::YAxisLeftTitle:
		if (Orientation == EKantanBarChartOrientation::Vertical && ValueAxisCfg.LeftBottomAxis.bEnabled && ValueAxisCfg.LeftBottomAxis.bShowTitle)
		{
			DrawYAxisTitle(Geometry, SnappedClippingRect, OutDrawElements, LayerId + BarChartLayers::ValueAxis, ValueAxisCfg, DetermineValueAxisMarkerData(PlotSpaceGeometry));
		}
		break;
	case EChartContentArea::YAxisRightTitle:
		if (Orientation == EKantanBarChartOrientation::Vertical && ValueAxisCfg.RightTopAxis.bEnabled && ValueAxisCfg.RightTopAxis.bShowTitle)
		{
			DrawYAxisTitle(Geometry, SnappedClippingRect, OutDrawElements, LayerId + BarChartLayers::ValueAxis, ValueAxisCfg, DetermineValueAxisMarkerData(PlotSpaceGeometry));
		}
		break;

	case EChartContentArea::YAxisLeft:
		if (Orientation == EKantanBarChartOrientation::Vertical && ValueAxisCfg.LeftBottomAxis.bEnabled)
		{
			DrawFixedAxis(
				Geometry,
				SnappedClippingRect,
				OutDrawElements,
				LayerId + BarChartLayers::ValueAxis,
				EAxis::Y,
				DetermineValueAxisTransform(PlotSpaceGeometry.GetLocalSize()),
				EChartAxisPosition::LeftBottom,
				DetermineValueAxisMarkerData(PlotSpaceGeometry),
				ValueAxisCfg.LeftBottomAxis.bShowMarkers,
				ValueAxisCfg.LeftBottomAxis.bShowLabels,
				ChartConstants::AxisMarkerLength,
				ChartConstants::AxisMarkerLabelGap
				);
		}
		else if (Orientation == EKantanBarChartOrientation::Horizontal && LabelPosition == EKantanBarLabelPosition::Standard)
		{
			DrawCategoryAxis(
				Area,
				Args,
				Geometry,
				PlotSpaceGeometry,
				MyClippingRect,
				OutDrawElements,
				LayerId + BarChartLayers::CategoryAxis,
				InWidgetStyle,
				bParentEnabled
				);
		}
		break;

	case EChartContentArea::YAxisRight:
		if (Orientation == EKantanBarChartOrientation::Vertical && ValueAxisCfg.RightTopAxis.bEnabled)
		{
			DrawFixedAxis(
				Geometry,
				SnappedClippingRect,
				OutDrawElements,
				LayerId + BarChartLayers::ValueAxis,
				EAxis::Y,
				DetermineValueAxisTransform(PlotSpaceGeometry.GetLocalSize()),
				EChartAxisPosition::RightTop,
				DetermineValueAxisMarkerData(PlotSpaceGeometry),
				ValueAxisCfg.RightTopAxis.bShowMarkers,
				ValueAxisCfg.RightTopAxis.bShowLabels,
				ChartConstants::AxisMarkerLength,
				ChartConstants::AxisMarkerLabelGap
				);
		}
		break;

	case EChartContentArea::XAxisBottom:
		if (Orientation == EKantanBarChartOrientation::Horizontal && ValueAxisCfg.LeftBottomAxis.bEnabled)
		{
			DrawFixedAxis(
				Geometry,
				SnappedClippingRect,
				OutDrawElements,
				LayerId + BarChartLayers::ValueAxis,
				EAxis::X,
				DetermineValueAxisTransform(PlotSpaceGeometry.GetLocalSize()),
				EChartAxisPosition::LeftBottom,
				DetermineValueAxisMarkerData(PlotSpaceGeometry),
				ValueAxisCfg.LeftBottomAxis.bShowMarkers,
				ValueAxisCfg.LeftBottomAxis.bShowLabels,
				ChartConstants::AxisMarkerLength,
				ChartConstants::AxisMarkerLabelGap
				);
		}
		else if (Orientation == EKantanBarChartOrientation::Vertical && LabelPosition == EKantanBarLabelPosition::Standard)
		{
			DrawCategoryAxis(
				Area,
				Args,
				Geometry,
				PlotSpaceGeometry,
				MyClippingRect,
				OutDrawElements,
				LayerId + BarChartLayers::CategoryAxis,
				InWidgetStyle,
				bParentEnabled
				);
		}
		break;

	case EChartContentArea::XAxisTop:
		if (Orientation == EKantanBarChartOrientation::Horizontal && ValueAxisCfg.RightTopAxis.bEnabled)
		{
			DrawFixedAxis(
				Geometry,
				SnappedClippingRect,
				OutDrawElements,
				LayerId + BarChartLayers::ValueAxis,
				EAxis::X,
				DetermineValueAxisTransform(PlotSpaceGeometry.GetLocalSize()),
				EChartAxisPosition::RightTop,
				DetermineValueAxisMarkerData(PlotSpaceGeometry),
				ValueAxisCfg.RightTopAxis.bShowMarkers,
				ValueAxisCfg.RightTopAxis.bShowLabels,
				ChartConstants::AxisMarkerLength,
				ChartConstants::AxisMarkerLabelGap
				);
		}
		break;

	case EChartContentArea::Plot:
	{
		auto const OverlaidLabelOffset = 5.0f;	// @TODO: Perhaps proportional amount?

		auto NumBars = GetNumCategories();

		auto const BarWidthProportion = CalcBarWidthProportion(NumBars);
		auto const GapWidthProportion = BarWidthProportion / BarToGapRatio;

		int32 ChartXComp = Orientation == EKantanBarChartOrientation::Vertical ? 0 : 1;
		int32 ChartYComp = Orientation == EKantanBarChartOrientation::Vertical ? 1 : 0;

		auto AvailableSize = Geometry.GetLocalSize();
		auto const BarWidth = AvailableSize[ChartXComp] * BarWidthProportion;
		auto const GapWidth = AvailableSize[ChartXComp] * GapWidthProportion;

		auto ValueRange = DetermineValueRange();
		float EffectiveMaxBarValue = ValueRange.Max;

		auto const MaxBarLength = AvailableSize[ChartYComp];

		for (int32 Idx = 0; Idx < NumBars; ++Idx)
		{
			auto Value = GetCategoryValue(Idx);
			auto const& CatStyle = GetCategoryStyle(GetCategoryId(Idx));

			auto ChartSpaceBarTranslation = FVector2D(BarWidth * Idx + GapWidth * (0.5f + Idx), 0.0f);// RequiredLabelSize);
			auto ChartSpaceBarSize = MapChartToLocal_Size(FVector2D(BarWidth, (Value / EffectiveMaxBarValue) * MaxBarLength));
			auto BarGeom = Geometry.MakeChild(
				ChartSpaceBarSize,
				FSlateLayoutTransform(MapChartToLocal(ChartSpaceBarTranslation, ChartSpaceBarSize, AvailableSize))
				);
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				LayerId + BarChartLayers::BarInterior,
				BarGeom.ToPaintGeometry(),
				BarInteriorBrush.Get(),
				//SnappedClippingRect,
				ESlateDrawEffect::None,
				CatStyle.Color.CopyWithNewOpacity(ChartStyle ? ChartStyle->BarOpacity : 1.0f));
			/*
			FSlateDrawElement::MakeBox(
			OutDrawElements,
			RetLayerId++,
			BarGeom.ToPaintGeometry(),
			BarBorderBrush.Get(),
			SnappedClippingRect,
			ESlateDrawEffect::None,
			CatStyle.Color.CopyWithNewOpacity(ChartStyle ? ChartStyle->BarOutlineOpacity : 1.0f));
			*/

			TArray< FVector2D > BarPoints;
			BarPoints.SetNumZeroed(4);
			auto const ChartYStart = Orientation == EKantanBarChartOrientation::Vertical ? ChartSpaceBarSize[ChartYComp] : 0.0f;
			auto const ChartYEnd = Orientation == EKantanBarChartOrientation::Vertical ? 0.0f : ChartSpaceBarSize[ChartYComp];
			BarPoints[0][ChartYComp] = ChartYStart;
			BarPoints[1][ChartYComp] = ChartYEnd;
			BarPoints[2][ChartYComp] = ChartYEnd;
			BarPoints[2][ChartXComp] = ChartSpaceBarSize[ChartXComp];
			BarPoints[3][ChartYComp] = ChartYStart;
			BarPoints[3][ChartXComp] = ChartSpaceBarSize[ChartXComp];

			FSlateDrawElement::MakeLines(
				OutDrawElements,
				LayerId + BarChartLayers::BarOutline,
				BarGeom.ToPaintGeometry(),
				BarPoints,
				//SnappedClippingRect,
				ESlateDrawEffect::None,
				CatStyle.Color.CopyWithNewOpacity(ChartStyle ? ChartStyle->BarOutlineOpacity : 1.0f),
				false,
				GetChartStyle()->BarOutlineThickness
				);
		}

		if (ValueExtentsDisplay != EKantanBarValueExtents::NoValueLines)
		{
			TArray< FVector2D > Points;
			Points.Add(MapChartToLocal(FVector2D(0.0f, 0.0f), FVector2D::ZeroVector, AvailableSize));
			Points.Add(MapChartToLocal(FVector2D(AvailableSize[ChartXComp], 0.0f), FVector2D::ZeroVector, AvailableSize));

			FSlateDrawElement::MakeLines(
				OutDrawElements,
				LayerId + BarChartLayers::ZeroLine,
				Geometry.ToPaintGeometry(),
				Points,
				//SnappedClippingRect.ExtendBy(ChartConstants::ChartClipRectExtension),
				ESlateDrawEffect::None,
				ChartStyle->ChartLineColor,
				false,
				ChartStyle->ChartLineThickness
			);
		}

		if (ValueExtentsDisplay == EKantanBarValueExtents::ZeroAndMaxLines)
		{
			TArray< FVector2D > Points;
			Points.Add(MapChartToLocal(FVector2D(0.0f, AvailableSize[ChartYComp]), FVector2D::ZeroVector, AvailableSize));
			Points.Add(MapChartToLocal(FVector2D(AvailableSize[ChartXComp], AvailableSize[ChartYComp]), FVector2D::ZeroVector, AvailableSize));

			FSlateDrawElement::MakeLines(
				OutDrawElements,
				LayerId + BarChartLayers::MaxValueLine,
				Geometry.ToPaintGeometry(),
				Points,
				//SnappedClippingRect.ExtendBy(ChartConstants::ChartClipRectExtension),
				ESlateDrawEffect::None,
				ChartStyle->ChartLineColor,
				false,
				ChartStyle->ChartLineThickness
			);
		}
	}
	break;
	}

	return LayerId;
}



