// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "IKantanChartsSlateModule.h"


class UKantanSimpleCartesianDatasource;
class UKantanSimpleCategoryDatasource;

struct FKantanCartesianChartStyle;
struct FKantanBarChartStyle;

namespace KantanCharts {

	class ICartesianChart;
	class ITimeSeriesChart;
	class IBarChart;
	class IChartLegend;

	/**
	* The public interface to Kantan Charts functionality usable with dynamic loading.
	*/
	class IKantanCharts
	{
	public:
		static inline const IKantanCharts& Get()
		{
			auto& Module = FModuleManager::LoadModuleChecked< IKantanChartsSlateModule >("KantanChartsSlate");
			return Module.GetKantanChartsInterface();
		}

		static inline bool IsAvailable()
		{
			return FModuleManager::Get().IsModuleLoaded("KantanChartsSlate");
		}

		virtual ~IKantanCharts() {}

	public:
		/*
		Public interface methods.
		*/
		virtual TSharedRef< ICartesianChart > CreateCartesianChart() const = 0;
		virtual TSharedRef< ITimeSeriesChart > CreateTimeSeriesChart() const = 0;
		virtual TSharedRef< IBarChart > CreateBarChart() const = 0;

		virtual TSharedRef< IChartLegend > CreateCartesianLegend() const = 0;

		virtual UKantanSimpleCartesianDatasource* CreateSimpleCartesianDatasource(UObject* Outer) const = 0;
		virtual UKantanSimpleCategoryDatasource* CreateSimpleCategoryDatasource(UObject* Outer) const = 0;

		virtual const FKantanCartesianChartStyle& GetDefaultCartesianChartEditorStyle() const = 0;
		virtual const FKantanBarChartStyle& GetDefaultBarChartEditorStyle() const = 0;

		virtual const FKantanCartesianChartStyle& GetDefaultCartesianChartGameStyle() const = 0;
		virtual const FKantanBarChartStyle& GetDefaultBarChartGameStyle() const = 0;

		virtual FName GetEditorStyleSetName() const = 0;
	};

}

