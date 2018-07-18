// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "KantanChartsSlateModule.h"
#include "Style/KantanChartsStyleSet.h"
#include "KantanChartsImpl.h"

#include "Styling/SlateStyleRegistry.h"
#include "Misc/ConfigCacheIni.h"


namespace KantanCharts {

	// @NOTE: New drawing mode fixes clipping issue, but is actually slightly less performant with large datasets.
	// Look at SMeshWidget if want to find a more optimal approach that (presumably) would also be properly clipped.
	// Can be switched by adding the following to a project's [Default]Game.ini:
	/*
	[KantanCharts]
	CustomPointDrawing=true/false
	*/
	bool FKantanChartsSlateModule::bCustomSeriesDrawing = false;

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


