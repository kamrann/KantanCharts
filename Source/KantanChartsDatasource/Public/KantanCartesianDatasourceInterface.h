// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "UObject/Interface.h"
#include "KantanCartesianDatapoint.h"
#include "KantanCartesianDatasourceInterface.generated.h"


UINTERFACE(Blueprintable, MinimalAPI)
class UKantanCartesianDatasourceInterface : public UInterface
{
	GENERATED_BODY()
};


class KANTANCHARTSDATASOURCE_API IKantanCartesianDatasourceInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Cartesian Datasource")
	int32 GetNumSeries() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Cartesian Datasource")
	FName GetSeriesId(int32 CatIdx) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Cartesian Datasource")
	FText GetSeriesName(int32 SeriesIdx) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Cartesian Datasource")
	TArray< FKantanCartesianDatapoint > GetSeriesDatapoints(int32 SeriesIdx) const;
};


