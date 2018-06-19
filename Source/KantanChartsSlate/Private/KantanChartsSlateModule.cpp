// Copyright (C) 2015-2017 Cameron Angus. All Rights Reserved.

#include "KantanChartsSlateModule.h"
#include "KantanChartsStyleSet.h"
#include "KantanChartsImpl.h"
#include "SlateStyleRegistry.h"
#include "ConfigCacheIni.h"


namespace KantanCharts {

	// @NOTE: Retaining legacy drawing for now, as it's slightly more performant.
	// Can be switched by adding the following to a project's [Default]Game.ini:
	/*
	[KantanCharts]
	CustomPointDrawing=false
	*/
	bool FKantanChartsSlateModule::bCustomSeriesDrawing = true;

	FKantanChartsSlateModule::FKantanChartsSlateModule(): CustomDrawingCmd(
		TEXT("KantanCharts.CustomPoints"), TEXT("Toggle custom drawing of datapoints (legacy implementation)"), FConsoleCommandWithArgsDelegate::CreateStatic(&FKantanChartsSlateModule::OnCustomDrawingCommand)
	)
	{}


	void FKantanChartsSlateModule::StartupModule()
	{
		// Hot reload hack
		FSlateStyleRegistry::UnRegisterSlateStyle(FKantanChartsStyleSet::GetStyleSetName());
		FKantanChartsStyleSet::Initialize();

		KC_Interface = MakeUnique< FKantanChartsImpl >();

		GConfig->GetBool(TEXT("KantanCharts"), TEXT("CustomPointDrawing"), bCustomSeriesDrawing, GGameIni);
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

	void FKantanChartsSlateModule::OnCustomDrawingCommand(const TArray<FString>& Arguments)
	{
		if (Arguments.Num())
		{
			bCustomSeriesDrawing = FCString::ToBool(*Arguments[0]);
			UE_LOG(LogTemp, Log, TEXT("KantanCharts legacy point drawing %s."), bCustomSeriesDrawing ? TEXT("is enabled") : TEXT("is disabled"));
		}
	}

}


IMPLEMENT_MODULE(KantanCharts::FKantanChartsSlateModule, KantanChartsSlate);


