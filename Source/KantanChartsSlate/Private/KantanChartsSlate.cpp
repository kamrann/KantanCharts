// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

#include "KantanChartsSlate.h"
#include "KantanChartsStyleSet.h"


class FKantanChartsSlateModule : public FDefaultModuleImpl
{
	virtual void StartupModule() override
	{
		// Hot reload hack
		FSlateStyleRegistry::UnRegisterSlateStyle(FKantanChartsStyleSet::GetStyleSetName());
		FKantanChartsStyleSet::Initialize();
	}

	virtual void ShutdownModule() override
	{
		FKantanChartsStyleSet::Shutdown();
	}
};


IMPLEMENT_MODULE(FKantanChartsSlateModule, KantanChartsSlate);


