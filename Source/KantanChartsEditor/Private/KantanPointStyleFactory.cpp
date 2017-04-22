// Copyright (C) 2015-2017 Cameron Angus. All Rights Reserved.

#include "KantanPointStyleFactory.h"
#include "KantanSeriesStyle.h"

#define LOCTEXT_NAMESPACE "KantanCharts"


UKantanPointStyleFactory::UKantanPointStyleFactory(const FObjectInitializer& OI) :
Super(OI)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UKantanPointStyle::StaticClass();
}

UObject* UKantanPointStyleFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	auto NewObjectAsset = NewObject< UObject >(InParent, Class, Name, Flags | RF_Transactional);
	return NewObjectAsset;
}

FText UKantanPointStyleFactory::GetDisplayName() const
{
	return LOCTEXT("PointStyleFactoryName", "Kantan Point Style");
}


#undef LOCTEXT_NAMESPACE


