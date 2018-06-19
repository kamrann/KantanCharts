// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "KantanCartesianDatapoint.generated.h"


USTRUCT(BlueprintType)
struct FKantanCartesianDatapoint
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FVector2D Coords;
};



