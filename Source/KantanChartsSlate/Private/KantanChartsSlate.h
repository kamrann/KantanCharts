#pragma once

#include "IKantanChartsSlateModule.h"
#include "UniquePtr.h"


namespace KantanCharts {

	class FKantanChartsSlateModule: public IKantanChartsSlateModule
	{
	public:
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;

	public:
		virtual const IKantanCharts& GetKantanChartsInterface() const override;

	protected:
		TUniquePtr< IKantanCharts > KC_Interface;
	};

}

