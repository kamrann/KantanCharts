// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "UObject/Interface.h"
#include "KantanCategoryDatasourceInterface.generated.h"


UINTERFACE(Blueprintable, MinimalAPI)
class UKantanCategoryDatasourceInterface : public UInterface
{
	GENERATED_BODY()
};


class KANTANCHARTSDATASOURCE_API IKantanCategoryDatasourceInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Category Datasource")
	int32 GetNumCategories() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Category Datasource")
	FName GetCategoryId(int32 CatIdx) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Category Datasource")
	FText GetCategoryName(int32 CatIdx) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Category Datasource")
	float GetCategoryValue(int32 CatIdx) const;
};


