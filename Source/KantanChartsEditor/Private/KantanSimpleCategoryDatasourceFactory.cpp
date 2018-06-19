// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "KantanSimpleCategoryDatasourceFactory.h"
#include "KantanSimpleCategoryDatasource.h"

#define LOCTEXT_NAMESPACE "KantanCharts"


UKantanSimpleCategoryDatasourceFactory::UKantanSimpleCategoryDatasourceFactory(const FObjectInitializer& OI):
Super(OI)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UKantanSimpleCategoryDatasource::StaticClass();
}

UObject* UKantanSimpleCategoryDatasourceFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	auto NewObjectAsset = NewObject< UObject >(InParent, Class, Name, Flags | RF_Transactional);
	return NewObjectAsset;
}

FText UKantanSimpleCategoryDatasourceFactory::GetDisplayName() const
{
	return LOCTEXT("SimpleCategoryDatasourceFactoryName", "Simple Category Datasource");
}


#undef LOCTEXT_NAMESPACE


