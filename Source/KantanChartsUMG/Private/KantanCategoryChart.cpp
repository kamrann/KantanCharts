// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "KantanCategoryChart.h"
#include "Style/KantanCategoryStyleSet.h"

#include "UObject/Package.h"
#include "UObject/ConstructorHelpers.h"

#define LOCTEXT_NAMESPACE "KantanCharts"


UKantanCategoryChart::UKantanCategoryChart(FObjectInitializer const& OI) : Super(OI)
{
	bAutoPerCategoryStyles = true;

	static ConstructorHelpers::FObjectFinder< UKantanCategoryStyleSet > DefaultCategoryStyles(
		TEXT("KantanCategoryStyleSet'/KantanCharts/Style/CategoryStyles/DefaultCategoryStyleSet.DefaultCategoryStyleSet'")
		);
	CategoryStyleSet = DefaultCategoryStyles.Object;
}

void UKantanCategoryChart::AddCategoryStyleOverride(FName CategoryId, FLinearColor Color)
{
	FCategoryStyleManualMapping NewMapping;
	NewMapping.CategoryId = CategoryId;
	NewMapping.Style.CategoryStyleId = GenerateCategoryStyleId();
	NewMapping.Style.Color = Color;
	ManualStyleMappings.Add(NewMapping);

	if (MyChart.IsValid())
	{
		UpdateSlateChartStyles();
	}
}

void UKantanCategoryChart::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	auto CategoryChart = GetCategoryChart();

	UObject* DatasourceToUse = nullptr;
#if WITH_EDITOR
	// In the editor, if at design time then use the preview datasource, unless the chart has a valid
	// datasource available and specifically wants it to be used.
	if (IsDesignTime() && bShowDataPreview && (UseDatasourceAtDesignTime() == false || InternalGetDatasource() == nullptr))
	{
		DatasourceToUse = InternalGetPreviewDatasource();
	}
	else
#endif
	{
		DatasourceToUse = InternalGetDatasource();
	}

	CategoryChart->SetDatasource(DatasourceToUse);
	CategoryChart->SetUseAutoPerCategoryStyles(bAutoPerCategoryStyles);

	UpdateSlateChartStyles();
}

void UKantanCategoryChart::UpdateSlateChartStyles()
{
	check(MyChart.IsValid());
	auto CategoryChart = GetCategoryChart();

	TArray< FKantanCategoryStyle > CatStyles;
	if (CategoryStyleSet)
	{
		CatStyles.Append(CategoryStyleSet->Styles);
	}
	for (auto& Override : ManualStyleMappings)
	{
		CatStyles.Add(Override.Style);
	}
	CategoryChart->SetCategoryStylesList(CatStyles);
	CategoryChart->SetManualCategoryStyleMappings(ConvertStyleMappings(ManualStyleMappings));
}

TMap< FName, FName > UKantanCategoryChart::ConvertStyleMappings(TArray< FCategoryStyleManualMapping > const& Mappings)
{
	TMap< FName, FName > Result;
	for (auto const& Mp : Mappings)
	{
		// @TODO: Check if the default TMap prevents duplicate keys
		Result.Add(Mp.CategoryId, Mp.Style.CategoryStyleId);
	}
	return Result;
}

FName UKantanCategoryChart::GenerateCategoryStyleId() const
{
	static const FString AutoStyleIdPrefix = TEXT("AutoStyleId__");

	int32 MaxNumber = 0;
	for (auto const& Mp : ManualStyleMappings)
	{
		FString IdStr = Mp.Style.CategoryStyleId.ToString();
		int32 Idx = IdStr.Find(AutoStyleIdPrefix, ESearchCase::CaseSensitive);
		if (Idx != 0)
		{
			continue;
		}

		auto NumStr = IdStr.RightChop(AutoStyleIdPrefix.Len());
		auto Num = FCString::Atoi(*NumStr);
		MaxNumber = FMath::Max(MaxNumber, Num);
	}

	return FName(*FString::Printf(TEXT("%s%i"), *AutoStyleIdPrefix, MaxNumber + 1));
}


#if WITH_EDITOR

void UKantanCategoryChart::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	static const FName NAME_ManualStyleMappings = GET_MEMBER_NAME_CHECKED(UKantanCategoryChart, ManualStyleMappings);

	if (PropertyChangedEvent.Property != NULL)
	{
		const FName PropName = PropertyChangedEvent.PropertyChain.GetHead()->GetValue()->GetFName();

		if (PropName == NAME_ManualStyleMappings)
		{
			// Auto assign a category style id to any newly added mapping
			for (auto& Override : ManualStyleMappings)
			{
				if (Override.Style.CategoryStyleId.IsNone())
				{
					Override.Style.CategoryStyleId = GenerateCategoryStyleId();
				}
			}
		}
	}

	Super::PostEditChangeChainProperty(PropertyChangedEvent);
}

UObject* UKantanCategoryChart::InternalGetPreviewDatasource() const
{
	return FindObject< UObject >(
		::GetTransientPackage(),
		TEXT("PreviewCategoryDatasource")
		);
}

#endif


#undef LOCTEXT_NAMESPACE


