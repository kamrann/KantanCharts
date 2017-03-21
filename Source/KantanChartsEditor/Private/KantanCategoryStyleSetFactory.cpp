// Copyright (C) 2015-2017 Cameron Angus. All Rights Reserved.

#include "KantanChartsEditor.h"
#include "KantanCategoryStyleSetFactory.h"
#include "KantanCategoryStyleSet.h"

#define LOCTEXT_NAMESPACE "KantanCharts"


UKantanCategoryStyleSetFactory::UKantanCategoryStyleSetFactory(const FObjectInitializer& OI) :
Super(OI)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UKantanCategoryStyleSet::StaticClass();
}

UObject* UKantanCategoryStyleSetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	auto NewObjectAsset = NewObject< UObject >(InParent, Class, Name, Flags | RF_Transactional);
	return NewObjectAsset;
}

FText UKantanCategoryStyleSetFactory::GetDisplayName() const
{
	return LOCTEXT("CategoryStyleSetFactoryName", "Kantan Category Style Set");
}


#undef LOCTEXT_NAMESPACE


