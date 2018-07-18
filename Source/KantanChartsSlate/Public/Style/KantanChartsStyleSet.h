// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "Styling/SlateStyle.h"
#include "CoreMinimal.h"


class KANTANCHARTSSLATE_API FKantanChartsStyleSet
{
public:
	static void Initialize();
	static void Shutdown();
	static void ReloadTextures();

	static const class ISlateStyle& Get();

	static FName GetStyleSetName();

private:
	static TSharedRef< class FSlateStyleSet > Create();

private:
	static TSharedPtr< class FSlateStyleSet > StyleInstance;
};


