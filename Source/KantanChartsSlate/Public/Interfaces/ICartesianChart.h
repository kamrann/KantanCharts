// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "Interfaces/IChartBase.h"
#include "KantanCartesianTypes.h"


struct FKantanCartesianChartStyle;
struct FKantanSeriesStyle;
struct FKantanCartesianPlotScale;
struct FCartesianAxisConfig;
struct FCartesianDataSnapshot;

namespace KantanCharts {

	class ICartesianChart: public virtual IChartBase
	{
	public:
		virtual void SetStyle(const FKantanCartesianChartStyle* InStyle) = 0;
		virtual void SetStyleFromAsset(USlateWidgetStyleContainerBase* InStyleAsset) = 0;
		virtual bool SetDatasource(UObject* InDatasource) = 0;
		virtual void SetUseAutoPerSeriesStyles(bool bEnabled) = 0;
		virtual void SetSeriesStylesList(TArray< FKantanSeriesStyle > const& Styles) = 0;
		virtual void LoadSeriesStylesList(const FSoftObjectPath& Styles) = 0;
		virtual void SetManualSeriesStyleMappings(TMap< FName, FName > const& Mappings) = 0;
		virtual void SetPlotScale(FKantanCartesianPlotScale const& Scaling) = 0;
		virtual void SetDataPointSize(EKantanDataPointSize::Type InSize) = 0;
		virtual void SetXAxisConfig(FCartesianAxisConfig const& InConfig) = 0;
		virtual void SetYAxisConfig(FCartesianAxisConfig const& InConfig) = 0;
		virtual void SetAxisTitlePadding(FMargin const& InPadding) = 0;
		virtual void SetAntialiasDataLines(bool bEnable) = 0;
		virtual void SetOnUpdatePlotScale(FOnUpdatePlotScale Delegate) = 0;

		virtual void EnableSeries(FName Id, bool bEnable) = 0;
		virtual void ConfigureSeries(FName Id, bool bDrawPoints, bool bDrawLines) = 0;
		virtual void SetSeriesStyle(FName Id, FName StyleId) = 0;
		virtual void ResetSeries(FName Id = NAME_None) = 0;
//		virtual void ResetSeriesNotInDatasource() = 0;

		virtual bool IsSeriesEnabled(FName Id) const = 0;
		virtual bool IsSeriesShowingLines(FName Id) const = 0;
		virtual bool IsSeriesShowingPoints(FName Id) const = 0;

		virtual FKantanSeriesStyle const& GetSeriesStyle(FName SeriesId) const = 0;
		virtual FCartesianDataSnapshot const& GetCurrentSnapshot() const = 0;
	};

}

