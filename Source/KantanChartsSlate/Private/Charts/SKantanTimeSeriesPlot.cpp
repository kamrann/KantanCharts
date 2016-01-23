// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

#include "KantanChartsSlate.h"
#include "SKantanTimeSeriesPlot.h"
#include "FloatRoundingLevel.h"

#include <algorithm>


void SKantanTimeSeriesPlot::Construct(const FArguments& InArgs)
{
	SKantanCartesianChart::Construct(
		SKantanCartesianChart::FArguments()
		.Style(InArgs._Style)
		.Datasource(InArgs._Datasource)
		.UpdateTickRate(InArgs._UpdateTickRate)
		);

	SetDisplayTimeRange(30.0f);
	SetRoundTimeRange(false);
}

void SKantanTimeSeriesPlot::SetDisplayTimeRange(float InRange)
{
	DisplayTimeRange = InRange;
}

void SKantanTimeSeriesPlot::SetRoundTimeRange(bool bInRound)
{
	bRoundTimeRange = bInRound;
}

void SKantanTimeSeriesPlot::SetLowerValueBound(FCartesianRangeBound const& InBound)
{
	LowerValueBound = InBound;
}

void SKantanTimeSeriesPlot::SetUpperValueBound(FCartesianRangeBound const& InBound)
{
	UpperValueBound = InBound;
}

void SKantanTimeSeriesPlot::OnActiveTick(double InCurrentTime, float InDeltaTime)
{
	SKantanCartesianChart::OnActiveTick(InCurrentTime, InDeltaTime);

	if (Datasource != nullptr)
	{
		PlotScale.Type = ECartesianScalingType::FixedRange;
		auto XSpan = GetDataAxisSpan(EAxis::X);
		PlotScale.RangeX = ValidateAxisDisplayRange(XSpan);
		if (DisplayTimeRange > 0.0f)
		{
			PlotScale.RangeX.Max = FMath::Max(PlotScale.RangeX.Max, PlotScale.RangeX.Min + DisplayTimeRange);
			PlotScale.RangeX.Min = FMath::Max(PlotScale.RangeX.Min, PlotScale.RangeX.Max - DisplayTimeRange);
		}

		if (bRoundTimeRange)
		{
			// @TODO: for now just rounding to about 1 tenth of the range
			auto RL = FFloatRoundingLevel::MakeFromMinimumStepSize(PlotScale.RangeX.Size() / 10);
			PlotScale.RangeX.Max = RL.RoundUp(PlotScale.RangeX.Max).GetFloatValue();
			if (DisplayTimeRange > 0.0f)
			{
				PlotScale.RangeX.Min = PlotScale.RangeX.Max - DisplayTimeRange;
				//RL.RoundDown(PlotScale.RangeX.Min).GetFloatValue();
			}
		}

		FCartesianAxisRange YSpan;

		switch (LowerValueBound.Type)
		{
		case ECartesianRangeBoundType::FixedValue:
			YSpan.Min = LowerValueBound.FixedBoundValue;
			break;

		case ECartesianRangeBoundType::FitToData:
		case ECartesianRangeBoundType::FitToDataRounded:
		{
			auto FitSpan = GetDataAxisSpan(EAxis::Y);
			YSpan.Min = FitSpan.Min;
		}
		break;
		}

		switch (UpperValueBound.Type)
		{
		case ECartesianRangeBoundType::FixedValue:
			YSpan.Max = UpperValueBound.FixedBoundValue;
			break;

		case ECartesianRangeBoundType::FitToData:
		case ECartesianRangeBoundType::FitToDataRounded:
		{
			auto FitSpan = GetDataAxisSpan(EAxis::Y);
			YSpan.Max = FitSpan.Max;
		}
		break;
		}

		PlotScale.RangeY = ValidateAxisDisplayRange(YSpan);
		// @TODO: for now just rounding to about 1 tenth of the range
		auto RL = FFloatRoundingLevel::MakeFromMinimumStepSize(PlotScale.RangeY.Size() / 10);

		if (LowerValueBound.Type == ECartesianRangeBoundType::FitToDataRounded)
		{
			PlotScale.RangeY.Min = RL.RoundDown(PlotScale.RangeY.Min).GetFloatValue();
		}

		if (UpperValueBound.Type == ECartesianRangeBoundType::FitToDataRounded)
		{
			PlotScale.RangeY.Max = RL.RoundUp(PlotScale.RangeY.Max).GetFloatValue();
		}
	}
}

void SKantanTimeSeriesPlot::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SKantanCartesianChart::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

FCartesianAxisRange SKantanTimeSeriesPlot::GetDataAxisSpan(EAxis::Type Axis) const
{
	FCartesianAxisRange Range(FLT_MAX, -FLT_MAX);

	int32 AxisIdx = Axis == EAxis::X ? 0 : 1;
	auto NumSeries = GetNumSeries();
	for (int32 Idx = 0; Idx < NumSeries; ++Idx)
	{
		auto Id = GetSeriesId(Idx);
		if (IsSeriesEnabled(Id) == false)
		{
			continue;
		}

		auto Datapoints = GetSeriesDatapoints(Idx);
		for (auto const& Pnt : Datapoints)
		{
			auto const AxisValue = Pnt.Coords[AxisIdx];
			
			if (AxisValue < Range.Min)
			{
				Range.Min = AxisValue;
			}
			if (AxisValue > Range.Max)
			{
				Range.Max = AxisValue;
			}
		}
	}

	return Range.IsInversed() ? FCartesianAxisRange(0.0f, 0.0f) : Range;
}

FCartesianAxisRange SKantanTimeSeriesPlot::ValidateAxisDisplayRange(FCartesianAxisRange InRange)
{
	// Normalize
	if (InRange.Min > InRange.Max)
	{
		Swap(InRange.Min, InRange.Max);
	}

	// Disallow zero sized range
	if (InRange.IsZero())
	{
		//InRange.Min -= 1.0f;
		InRange.Max += 1.0f;
	}

	return InRange;
}

void SKantanTimeSeriesPlot::GetLinePointsToDraw(
	TArray< FKantanCartesianDatapoint > const& InPoints,
	FCartesianAxisRange const& RangeX,
	FCartesianAxisRange const& RangeY,
	TArray< FVector2D >& OutPoints) const
{
	// Assume points are ordered by x value, and search for first and last points within the plot range
	struct ComparePred
	{
		inline bool operator() (FKantanCartesianDatapoint const& A, FKantanCartesianDatapoint const& B) const
		{
			return A.Coords.X < B.Coords.X;
		};

		inline bool operator() (FKantanCartesianDatapoint const& A, float B) const
		{
			return A.Coords.X < B;
		};

		inline bool operator() (float A, FKantanCartesianDatapoint const& B) const
		{
			return A < B.Coords.X;
		};
	};

	int32 Lower = 0;
	int32 Upper = 0;

	if (InPoints.Num())
	{
		Lower = std::lower_bound(&InPoints[0], &InPoints[0] + InPoints.Num(), RangeX.Min, ComparePred()) - &InPoints[0];
		Upper = (Lower == InPoints.Num() ? Lower : (std::upper_bound(&InPoints[Lower], &InPoints[0] + InPoints.Num(), RangeX.Max, ComparePred()) - &InPoints[0]));

		// Include point preceding the first in range, if exists
		if (Lower != 0 && Lower != InPoints.Num())
		{
			Lower -= 1;
		}

		// Include the last point
		if (Upper != InPoints.Num())
		{
			Upper += 1;
		}
	}

	auto const Count = Upper - Lower;
	OutPoints.SetNumUninitialized(Count);
	for (int32 Idx = 0; Idx < Count; ++Idx)
	{
		OutPoints[Idx] = InPoints[Lower + Idx].Coords;
	}
}


