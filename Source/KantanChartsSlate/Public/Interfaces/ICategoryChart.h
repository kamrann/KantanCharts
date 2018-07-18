// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "Interfaces/IChartBase.h"


struct FKantanCategoryStyle;

namespace KantanCharts {

	class ICategoryChart: public virtual IChartBase
	{
	public:
		virtual bool SetDatasource(UObject* InDatasource) = 0;
		virtual void SetUseAutoPerCategoryStyles(bool bEnabled) = 0;
		virtual void SetCategoryStylesList(TArray< FKantanCategoryStyle > const& Styles) = 0;
		virtual void LoadCategoryStylesList(const FSoftObjectPath& Styles) = 0;
		virtual void SetManualCategoryStyleMappings(TMap< FName, FName > const& Mappings) = 0;
		virtual void ResetCategoryStyleMappings() = 0;
	};

}

