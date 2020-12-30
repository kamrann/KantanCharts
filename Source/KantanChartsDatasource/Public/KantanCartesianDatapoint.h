// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "KantanCartesianDatapoint.generated.h"


// Slightly more complicated struct for FLinearColor for semantics' sake
USTRUCT(BlueprintType)
struct FKantanDrawColor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Color")
	FLinearColor Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Color")
	bool DoOverrideColor;
};

USTRUCT(BlueprintType)
struct FKantanCartesianDatapoint
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FVector2D Coords;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FKantanDrawColor OverrideColor;
	
};



