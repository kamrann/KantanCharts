// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

#pragma once


class KANTANCHARTSSLATE_API FKantanChartsStyleSet
{
public:
	static void Initialize();
	static void Shutdown();
	static void ReloadTextures();

	static const class ISlateStyle& Get();

	static FString InContent(const FString& RelativePath, const TCHAR* Extension);
	static FName GetStyleSetName();

private:
	static TSharedRef< class FSlateStyleSet > Create();

private:
	static TSharedPtr< class FSlateStyleSet > StyleInstance;
};


