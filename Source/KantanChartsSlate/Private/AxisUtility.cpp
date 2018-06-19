// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "AxisUtility.h"
#include "KantanCartesianTypes.h"


namespace AxisUtil {

	FFloatRoundingLevel DetermineAxisRoundingLevel(
		FAxisTransform const& ValueToChartAxisTransform,
		float MinChartSpaceMarkerSeparation
		)
	{
		// Start with the default unit rounding level (10^0)
		FFloatRoundingLevel RLevel(0);

		// Since this function loops looking for the most suitable rounding level, there is a danger of infinite
		// looping if the transform is dodgy. So bail right away if we find any NANs.
		if(ValueToChartAxisTransform.CheckValidity() == false)
		{
			return RLevel;
		}

		// Now we map a vector with length equal to the RL step size from cartesian space to plot space.
		// We then get the length of that mapped vector in plot space.
		float ChartSpaceSeparation = FMath::Abs(ValueToChartAxisTransform.MapLength(RLevel.GetStepValue()));

		// Now we decide whether the initial step size is too big or too small and continuously loop,
		// up or down, until we find one where the mapped length in plot space is of an acceptable size.
		int32 Iterations = 0;
		int32 const MaxIterations = 1000;	// Used an an extra check to prevent hanging the editor, just in case.
		if (ChartSpaceSeparation < MinChartSpaceMarkerSeparation)
		{
			while (ChartSpaceSeparation < MinChartSpaceMarkerSeparation && Iterations < MaxIterations)
			{
				RLevel.Increase();
				ChartSpaceSeparation = FMath::Abs(ValueToChartAxisTransform.MapLength(RLevel.GetStepValue()));
				++Iterations;
			}
		}
		else
		{
			while (ChartSpaceSeparation > MinChartSpaceMarkerSeparation && Iterations < MaxIterations)
			{
				RLevel.Decrease();
				ChartSpaceSeparation = FMath::Abs(ValueToChartAxisTransform.MapLength(RLevel.GetStepValue()));
				++Iterations;
			}
			RLevel.Increase();
		}

		return RLevel;
	}

	FAxisMarkerData DetermineAxisMarkerData(
		FFloatRoundingLevel const& RLevel,			// Rounding level to use for the axis markers
		FCartesianAxisRange const& AxisRange,		// Range of the axis, in value space
		int32 MaxLabelDigits						// Maximum number of digits to display on a marker label
		)
	{
		FAxisMarkerData Data;
		Data.RL = RLevel;

		// Iterate over all values along the rounding level between the min and max values of the axis range
		auto Start = Data.RL.RoundUp(AxisRange.Min);
		for (auto RoundedMarker = Start; RoundedMarker.GetFloatValue() < AxisRange.Max; ++RoundedMarker)
		{
			// Add the value to the markers list
			Data.MarkerValues.Add(RoundedMarker);
		}

		// See if we want to use an offset value (eg. Offset + n*10^p)
		if(AxisRange.Contains(0.0f) == false)
		{
			// Since we don't span 0, the smallest absolute value in the range will be either its min or max.
			const bool bIsPositive = AxisRange.Min > 0.0f;
			float SmallestSigned = bIsPositive ? AxisRange.Min : AxisRange.Max;
			float SmallestAbs = FMath::Abs(SmallestSigned);

			// Start at the power that will be sufficiently large such that rounding down 
			// SmallestAbs will yield zero.
			const int32 StartPower = FMath::CeilToInt(FMath::LogX(10.0f, SmallestAbs)) + 1;
			auto Power = StartPower;

			// @NOTE: Doesn't make sense for the power of the offset to be smaller than that 
			// of the step levels of the labels. There are some cases, where MaxLabelDigits is small
			// and the number of desired labels too large, where it just isn't possible to satisfy
			// the requirements.
			while(Power > RLevel.GetPower())
			{
				FFloatRoundingLevel OffsetRL(Power, 10);
				auto RoundedOffset = bIsPositive ? OffsetRL.RoundDown(SmallestSigned) : OffsetRL.RoundUp(SmallestSigned);

				// Expecting that the first power chosen should result in no offset.
				ensure(Power != StartPower || RoundedOffset.IsZero());

				bool bCanRepresent = true;
				for(auto const& Mkr : Data.MarkerValues)
				{
					auto RelativeMarker = Mkr.RelativeTo(RoundedOffset);
					auto ReqDigits = RelativeMarker.RequiredDigits();
					if(ReqDigits > MaxLabelDigits)
					{
						bCanRepresent = false;
						break;
					}
				}

				if(bCanRepresent)
				{
					if(RoundedOffset.IsZero() == false)
					{
						Data.Offset = RoundedOffset;
					}
					break;
				}

				--Power;
			}
		}

		for(auto const& Mkr : Data.MarkerValues)
		{
			auto RelativeMkr = Data.Offset.IsSet() ?
				Mkr.RelativeTo(Data.Offset.GetValue()) :
				Mkr;
			// See if this value requires a more extreme power than any previous, and if so update
			auto ReqPower = RelativeMkr.RequiredPowerForDisplay(MaxLabelDigits);
			if(FMath::Abs(ReqPower) > FMath::Abs(Data.DisplayPower))
			{
				Data.DisplayPower = ReqPower;
			}
		}

		return Data;
	}

}


