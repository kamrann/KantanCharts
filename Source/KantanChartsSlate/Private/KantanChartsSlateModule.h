#pragma once

#include "IKantanChartsSlateModule.h"
#include "Templates/UniquePtr.h"
#include "HAL/IConsoleManager.h"


namespace KantanCharts {

	class FKantanChartsSlateModule: public IKantanChartsSlateModule
	{
	public:
		FKantanChartsSlateModule();

		virtual void StartupModule() override;
		virtual void ShutdownModule() override;

	public:
		virtual const IKantanCharts& GetKantanChartsInterface() const override;

		// Module internal public use 
		static bool bCustomSeriesDrawing;

	protected:
		static void OnCustomDrawingCommand(const TArray<FString>& Arguments);

	protected:
		TUniquePtr< IKantanCharts > KC_Interface;

		FAutoConsoleCommand CustomDrawingCmd;
	};

}

