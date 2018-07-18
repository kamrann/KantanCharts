// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "Interfaces/ICartesianChart.h"


namespace KantanCharts {

	class ITimeSeriesChart: public virtual ICartesianChart
	{
	public:
		virtual void SetFixedTimeRange(TOptional< float > InRange) = 0;
		//virtual void SetRoundTimeRange(bool bInRound) = 0;
		virtual void SetLowerValueBound(FCartesianRangeBound const& InBound) = 0;
		virtual void SetUpperValueBound(FCartesianRangeBound const& InBound) = 0;
		virtual void SetLowerTimeBound(FCartesianRangeBound const& InBound) = 0;
		virtual void SetUpperTimeBound(FCartesianRangeBound const& InBound) = 0;
	};

}

