// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

#include "KantanChartsSlate.h"
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

			// See if this value requires a more extreme power than any previous, and if so update
			// @TODO: Look into this.
			auto ReqPower = RoundedMarker.RequiredPowerForDisplay(MaxLabelDigits);
			if (FMath::Abs(ReqPower) > FMath::Abs(Data.DisplayPower))
			{
				Data.DisplayPower = ReqPower;
			}
		}

		return Data;
	}

}


