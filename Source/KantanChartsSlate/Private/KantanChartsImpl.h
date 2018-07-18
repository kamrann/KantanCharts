#pragma once

#include "Interfaces/IKantanChartsInterface.h"


namespace KantanCharts {

	class FKantanChartsImpl: public IKantanCharts
	{
	public:
		virtual TSharedRef< ICartesianChart > CreateCartesianChart() const override;
		virtual TSharedRef< ITimeSeriesChart > CreateTimeSeriesChart() const override;
		virtual TSharedRef< IBarChart > CreateBarChart() const override;

		virtual TSharedRef< IChartLegend > CreateCartesianLegend() const override;

		virtual UKantanSimpleCartesianDatasource* CreateSimpleCartesianDatasource(UObject* Outer) const override;
		virtual UKantanSimpleCategoryDatasource* CreateSimpleCategoryDatasource(UObject* Outer) const override;

		virtual const FKantanCartesianChartStyle& GetDefaultCartesianChartEditorStyle() const override;
		virtual const FKantanBarChartStyle& GetDefaultBarChartEditorStyle() const override;

		virtual const FKantanCartesianChartStyle& GetDefaultCartesianChartGameStyle() const override;
		virtual const FKantanBarChartStyle& GetDefaultBarChartGameStyle() const override;

		virtual FName GetEditorStyleSetName() const override;
	};

}



