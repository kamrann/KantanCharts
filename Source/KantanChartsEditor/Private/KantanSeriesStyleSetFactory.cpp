// Copyright (C) 2015-2017 Cameron Angus. All Rights Reserved.

#include "KantanChartsEditor.h"
#include "KantanSeriesStyleSetFactory.h"
#include "KantanSeriesStyleSet.h"

#define LOCTEXT_NAMESPACE "KantanCharts"


UKantanSeriesStyleSetFactory::UKantanSeriesStyleSetFactory(const FObjectInitializer& OI) :
Super(OI)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UKantanSeriesStyleSet::StaticClass();
}

UObject* UKantanSeriesStyleSetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	auto NewObjectAsset = NewObject< UObject >(InParent, Class, Name, Flags | RF_Transactional);
	return NewObjectAsset;
}

FText UKantanSeriesStyleSetFactory::GetDisplayName() const
{
	return LOCTEXT("SeriesStyleSetFactoryName", "Kantan Series Style Set");
}


#undef LOCTEXT_NAMESPACE


