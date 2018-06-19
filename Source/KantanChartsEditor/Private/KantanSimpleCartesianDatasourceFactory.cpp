// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "KantanSimpleCartesianDatasourceFactory.h"
#include "KantanSimpleCartesianDatasource.h"

#define LOCTEXT_NAMESPACE "KantanCharts"


UKantanSimpleCartesianDatasourceFactory::UKantanSimpleCartesianDatasourceFactory(const FObjectInitializer& OI):
Super(OI)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UKantanSimpleCartesianDatasource::StaticClass();
}

UObject* UKantanSimpleCartesianDatasourceFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	auto NewObjectAsset = NewObject< UObject >(InParent, Class, Name, Flags | RF_Transactional);
	return NewObjectAsset;
}

FText UKantanSimpleCartesianDatasourceFactory::GetDisplayName() const
{
	return LOCTEXT("SimpleCartesianDatasourceFactoryName", "Simple Cartesian Datasource");
}


#undef LOCTEXT_NAMESPACE


