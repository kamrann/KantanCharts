// Copyright (C) 2015-2017 Cameron Angus. All Rights Reserved.

#include "SKantanTimeSeriesPlot.h"
#include "FloatRoundingLevel.h"

#include <algorithm>
#include <cmath>
#include <limits>


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

	SetOnUpdatePlotScale(FOnUpdatePlotScale::CreateSP(this, &SKantanTimeSeriesPlot::DeterminePlotScale));
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
}

void SKantanTimeSeriesPlot::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SKantanCartesianChart::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

FCartesianAxisRange SKantanTimeSeriesPlot::ValidateAxisDisplayRange(FCartesianAxisRange InRange)
{
	if(InRange.ContainsNaNOrInf())
	{
		// @TODO: Log error
		return FCartesianAxisRange(-1.0f, 1.0f);
	}

	// Normalize
	if (InRange.Min > InRange.Max)
	{
		Swap(InRange.Min, InRange.Max);
	}

	// Disallow zero sized range
	if (InRange.IsZero())
	{
		if(InRange.Max == 0.0f)
		{
			InRange.Min = -1.0f;
			InRange.Max = 1.0f;
		}
		else
		{
			// Incr max
			{
				int n = (int)std::floor(std::log10(std::abs(InRange.Max)));
				// Loop until we find an incremented value that is representable as a value distinct from Max
				auto NewMax = InRange.Max;
				for(; NewMax == InRange.Max && (NewMax == 0.0f || std::isnormal(NewMax)); ++n)
				{
					auto Incr = std::pow(10.0f, n);
					NewMax = InRange.Max + Incr;
				}

				if(std::isnormal(NewMax))
				{
					InRange.Max = NewMax;
				}
			}

			// Decr min
			{
				int n = (int)std::floor(std::log10(std::abs(InRange.Min)));
				// Loop until we find an incremented value that is representable as a value distinct from Max
				auto NewMin = InRange.Min;
				for(; NewMin == InRange.Min && (NewMin == 0.0f || std::isnormal(NewMin)); ++n)
				{
					auto Decr = std::pow(10.0f, n);
					NewMin = InRange.Min - Decr;
				}

				if(std::isnormal(NewMin))
				{
					InRange.Min = NewMin;
				}
			}

			ensure(InRange.IsZero() == false);
		}
	}

	return InRange;
}

FKantanCartesianPlotScale SKantanTimeSeriesPlot::DeterminePlotScale(const FCartesianDataSnapshot& Snapshot, const TArray< int32 >& Enabled) const
{
	FKantanCartesianPlotScale NewPlotScale;

	NewPlotScale.Type = ECartesianScalingType::FixedRange;
	auto XSpan = Snapshot.GetDataRange(EAxis::X, true, Enabled);
	NewPlotScale.RangeX = ValidateAxisDisplayRange(XSpan);
	if(DisplayTimeRange > 0.0f)
	{
		NewPlotScale.RangeX.Max = FMath::Max(NewPlotScale.RangeX.Max, NewPlotScale.RangeX.Min + DisplayTimeRange);
		NewPlotScale.RangeX.Min = FMath::Max(NewPlotScale.RangeX.Min, NewPlotScale.RangeX.Max - DisplayTimeRange);
	}

	if(bRoundTimeRange)
	{
		// @TODO: for now just rounding to about 1 tenth of the range
		auto RL = FFloatRoundingLevel::MakeFromMinimumStepSize(NewPlotScale.RangeX.Size() / 10);
		NewPlotScale.RangeX.Max = RL.RoundUp(NewPlotScale.RangeX.Max).GetFloatValue();
		if(DisplayTimeRange > 0.0f)
		{
			NewPlotScale.RangeX.Min = NewPlotScale.RangeX.Max - DisplayTimeRange;
			//RL.RoundDown(NewPlotScale.RangeX.Min).GetFloatValue();
		}
	}

	TOptional< FCartesianAxisRange > DataYSpan;
	FCartesianAxisRange YSpan;

	switch(LowerValueBound.Type)
	{
		case ECartesianRangeBoundType::FixedValue:
		YSpan.Min = LowerValueBound.FixedBoundValue;
		break;

		case ECartesianRangeBoundType::FitToData:
		case ECartesianRangeBoundType::FitToDataRounded:
		{
			if(!DataYSpan.IsSet())
			{
				DataYSpan = Snapshot.GetDataRange(EAxis::Y, false, Enabled);
			}
			YSpan.Min = DataYSpan->Min;
		}
		break;
	}

	switch(UpperValueBound.Type)
	{
		case ECartesianRangeBoundType::FixedValue:
		YSpan.Max = UpperValueBound.FixedBoundValue;
		break;

		case ECartesianRangeBoundType::FitToData:
		case ECartesianRangeBoundType::FitToDataRounded:
		{
			if(!DataYSpan.IsSet())
			{
				DataYSpan = Snapshot.GetDataRange(EAxis::Y, false, Enabled);
			}
			YSpan.Max = DataYSpan->Max;
		}
		break;
	}

	NewPlotScale.RangeY = ValidateAxisDisplayRange(YSpan);
	// @TODO: for now just rounding to about 1 tenth of the range
	auto RL = FFloatRoundingLevel::MakeFromMinimumStepSize(NewPlotScale.RangeY.Size() / 10);

	if(LowerValueBound.Type == ECartesianRangeBoundType::FitToDataRounded)
	{
		NewPlotScale.RangeY.Min = RL.RoundDown(NewPlotScale.RangeY.Min).GetFloatValue();
	}

	if(UpperValueBound.Type == ECartesianRangeBoundType::FitToDataRounded)
	{
		NewPlotScale.RangeY.Max = RL.RoundUp(NewPlotScale.RangeY.Max).GetFloatValue();
	}

	return NewPlotScale;
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


