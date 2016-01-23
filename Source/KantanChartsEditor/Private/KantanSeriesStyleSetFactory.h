// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

#pragma once

#include "UnrealEd.h"
#include "KantanSeriesStyleSetFactory.generated.h"


/*
Factory class for series style set asset.
*/
UCLASS()
class UKantanSeriesStyleSetFactory: public UFactory
{
	GENERATED_BODY()

public:
	UKantanSeriesStyleSetFactory(FObjectInitializer const&);

public:
	// UFactory interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual FText GetDisplayName() const override;
	// End of UFactory interface
};


