// Copyright (C) 2015-2017 Cameron Angus. All Rights Reserved.

#include "KantanChartsSlateModule.h"
#include "KantanChartsStyleSet.h"
#include "KantanChartsImpl.h"
#include "SlateStyleRegistry.h"


namespace KantanCharts {

	void FKantanChartsSlateModule::StartupModule()
	{
		// Hot reload hack
		FSlateStyleRegistry::UnRegisterSlateStyle(FKantanChartsStyleSet::GetStyleSetName());
		FKantanChartsStyleSet::Initialize();

		KC_Interface = MakeUnique< FKantanChartsImpl >();
	}

	void FKantanChartsSlateModule::ShutdownModule()
	{
		KC_Interface.Reset();

		FKantanChartsStyleSet::Shutdown();
	}

	const IKantanCharts& FKantanChartsSlateModule::GetKantanChartsInterface() const
	{
		check(KC_Interface.IsValid());
		return *KC_Interface;
	}

}


IMPLEMENT_MODULE(KantanCharts::FKantanChartsSlateModule, KantanChartsSlate);


