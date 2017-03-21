// Copyright (C) 2015-2017 Cameron Angus. All Rights Reserved.

#pragma once

#include "UnrealEd.h"
#include "KantanCategoryStyleSetFactory.generated.h"


/*
Factory class for category style set asset.
*/
UCLASS()
class UKantanCategoryStyleSetFactory: public UFactory
{
	GENERATED_BODY()

public:
	UKantanCategoryStyleSetFactory(FObjectInitializer const&);

public:
	// UFactory interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual FText GetDisplayName() const override;
	// End of UFactory interface
};


