#pragma once

#include "Modules/ModuleManager.h"


namespace KantanCharts {

	class IKantanCharts;

	class IKantanChartsSlateModule: public IModuleInterface
	{
	public:
		virtual const IKantanCharts& GetKantanChartsInterface() const = 0;
	};

}

