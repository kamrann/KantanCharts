// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"
#include "KantanCategoryChart.h"


class FKantanCategoryChartCustomization: public IDetailCustomization
{
public:
	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	//

	static TSharedRef< IDetailCustomization > MakeInstance();
};


