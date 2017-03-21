// Copyright (C) 2015-2017 Cameron Angus. All Rights Reserved.

#pragma once

#include "UnrealEd.h"
#include "KantanSimpleCategoryDatasourceFactory.generated.h"


/*
Factory class for simple category data source asset.
*/
UCLASS()
class UKantanSimpleCategoryDatasourceFactory: public UFactory
{
	GENERATED_BODY()

public:
	UKantanSimpleCategoryDatasourceFactory(FObjectInitializer const&);

public:
	// UFactory interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual FText GetDisplayName() const override;
	// End of UFactory interface
};


