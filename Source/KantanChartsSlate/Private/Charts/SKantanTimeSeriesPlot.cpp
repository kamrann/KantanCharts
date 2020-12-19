// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "Charts/SKantanTimeSeriesPlot.h"
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

	SetFixedTimeRange(30.0f);
	//SetRoundTimeRange(false);

	LowerValueBound.SetFitToDataRounded();
	UpperValueBound.SetFitToDataRounded();
	LowerTimeBound.SetFitToDataRounded();
	UpperTimeBound.SetFitToDataRounded();
	SetOnUpdatePlotScale(FOnUpdatePlotScale::CreateSP(this, &SKantanTimeSeriesPlot::DeterminePlotScale));
}

void SKantanTimeSeriesPlot::SetFixedTimeRange(TOptional< float > InRange)
{
	FixedTimeRange = InRange;
	if(FixedTimeRange.IsSet())
	{
		const float MinimumFixedTimeRange = 1.0e-6f;	// @TODO: don't hard code
		FixedTimeRange = FMath::Max(FixedTimeRange.GetValue(), MinimumFixedTimeRange);
	}
}
/*
void SKantanTimeSeriesPlot::SetRoundTimeRange(bool bInRound)
{
	bRoundTimeRange = bInRound;
}
*/
void SKantanTimeSeriesPlot::SetLowerValueBound(FCartesianRangeBound const& InBound)
{
	LowerValueBound = InBound;
}

void SKantanTimeSeriesPlot::SetUpperValueBound(FCartesianRangeBound const& InBound)
{
	UpperValueBound = InBound;
}

void SKantanTimeSeriesPlot::SetLowerTimeBound(FCartesianRangeBound const& InBound)
{
	LowerTimeBound = InBound;
}

void SKantanTimeSeriesPlot::SetUpperTimeBound(FCartesianRangeBound const& InBound)
{
	UpperTimeBound = InBound;
}

void SKantanTimeSeriesPlot::SetExtendValueRangeToZero(bool bExtendToZero)
{
	bExtendValueRangeToZero = bExtendToZero;
}

void SKantanTimeSeriesPlot::OnActiveTick(double InCurrentTime, float InDeltaTime)
{
	SKantanCartesianChart::OnActiveTick(InCurrentTime, InDeltaTime);
}

void SKantanTimeSeriesPlot::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SKantanCartesianChart::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

FKantanCartesianPlotScale SKantanTimeSeriesPlot::DeterminePlotScale(const FCartesianDataSnapshot& Snapshot, const TArray< int32 >& Enabled) const
{
	// @TODO: for now just rounding to about 1 tenth of the range
	const float RoundingProportion = 0.1f;

	FKantanCartesianPlotScale NewPlotScale;
	NewPlotScale.Type = ECartesianScalingType::FixedRange;
	
	// Y (value) axis
	{
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
					DataYSpan = Snapshot.GetDataRange(EAxis::Y, false, Enabled, bExtendValueRangeToZero);
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
					DataYSpan = Snapshot.GetDataRange(EAxis::Y, false, Enabled, bExtendValueRangeToZero);
				}
				YSpan.Max = DataYSpan->Max;
			}
			break;
		}

		// Need to validate before passing into rounding calcs, to avoid infinite looping.
		YSpan = ValidateAxisDisplayRange(YSpan);
		
		if(LowerValueBound.Type == ECartesianRangeBoundType::FitToDataRounded || UpperValueBound.Type == ECartesianRangeBoundType::FitToDataRounded)
		{
			auto RL = FFloatRoundingLevel::MakeFromMinimumStepSize(YSpan.Size() * RoundingProportion);

			if(LowerValueBound.Type == ECartesianRangeBoundType::FitToDataRounded)
			{
				YSpan.Min = RL.RoundDown(YSpan.Min).GetFloatValue();
			}

			if(UpperValueBound.Type == ECartesianRangeBoundType::FitToDataRounded)
			{
				YSpan.Max = RL.RoundUp(YSpan.Max).GetFloatValue();
			}

			// Extra validation just to be safe
			YSpan = ValidateAxisDisplayRange(YSpan);
		}

		NewPlotScale.RangeY = YSpan;
	}

	// X (time) axis
	{
		TOptional< FCartesianAxisRange > DataXSpan;
		FCartesianAxisRange XSpan;

		switch(UpperTimeBound.Type)
		{
			case ECartesianRangeBoundType::FixedValue:
			XSpan.Max = UpperTimeBound.FixedBoundValue;
			break;

			case ECartesianRangeBoundType::FitToData:
			case ECartesianRangeBoundType::FitToDataRounded:
			{
				if(!DataXSpan.IsSet())
				{
					DataXSpan = Snapshot.GetDataRange(EAxis::X, true, Enabled, false);
				}
				XSpan.Max = DataXSpan->Max;
			}
			break;
		}

		if(FixedTimeRange.IsSet())
		{
			if(!DataXSpan.IsSet())
			{
				DataXSpan = Snapshot.GetDataRange(EAxis::X, true, Enabled, false);
			}

			// For fixed time span, ensure the max is at least as big as the time span.
			XSpan.Max = FMath::Max(XSpan.Max, FixedTimeRange.GetValue());

			// Then set the lower bound from the upper one.
			XSpan.Min = XSpan.Max - FixedTimeRange.GetValue();
		}
		else
		{
			switch(LowerTimeBound.Type)
			{
				case ECartesianRangeBoundType::FixedValue:
				XSpan.Min = LowerTimeBound.FixedBoundValue;
				break;

				case ECartesianRangeBoundType::FitToData:
				case ECartesianRangeBoundType::FitToDataRounded:
				{
					if(!DataXSpan.IsSet())
					{
						DataXSpan = Snapshot.GetDataRange(EAxis::X, true, Enabled, false);
					}
					XSpan.Min = DataXSpan->Min;
				}
				break;
			}
		}

		XSpan = ValidateAxisDisplayRange(XSpan);
		
		if((!FixedTimeRange.IsSet() && LowerTimeBound.Type == ECartesianRangeBoundType::FitToDataRounded) || UpperTimeBound.Type == ECartesianRangeBoundType::FitToDataRounded)
		{
			auto RL = FFloatRoundingLevel::MakeFromMinimumStepSize(XSpan.Size() * RoundingProportion);

			if(UpperTimeBound.Type == ECartesianRangeBoundType::FitToDataRounded)
			{
				XSpan.Max = RL.RoundUp(XSpan.Max).GetFloatValue();

				if(FixedTimeRange.IsSet())
				{
					XSpan.Min = XSpan.Max - FixedTimeRange.GetValue();
				}
			}

			if(!FixedTimeRange.IsSet() && LowerTimeBound.Type == ECartesianRangeBoundType::FitToDataRounded)
			{
				XSpan.Min = RL.RoundDown(XSpan.Min).GetFloatValue();
			}

			XSpan = ValidateAxisDisplayRange(XSpan);
		}

		NewPlotScale.RangeX = XSpan;

		/*	if(bRoundTimeRange)
		{
		auto RL = FFloatRoundingLevel::MakeFromMinimumStepSize(NewPlotScale.RangeX.Size() * RoundingProportion);
		NewPlotScale.RangeX.Max = RL.RoundUp(NewPlotScale.RangeX.Max).GetFloatValue();
		if(DisplayTimeRange > 0.0f)
		{
		NewPlotScale.RangeX.Min = NewPlotScale.RangeX.Max - DisplayTimeRange;
		//RL.RoundDown(NewPlotScale.RangeX.Min).GetFloatValue();
		}
		}
		*/
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


