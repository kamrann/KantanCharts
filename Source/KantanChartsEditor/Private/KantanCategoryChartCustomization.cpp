// Copyright (C) 2015-2017 Cameron Angus. All Rights Reserved.

#include "KantanCategoryChartCustomization.h"
#include "PropertyEditing.h"
#include "KantanCategoryChart.h"

#define LOCTEXT_NAMESPACE "KantanChartsEditor"


TSharedRef< IDetailCustomization > FKantanCategoryChartCustomization::MakeInstance()
{
	return MakeShareable(new FKantanCategoryChartCustomization);
}

void FKantanCategoryChartCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	auto& ConfigCat = DetailBuilder.EditCategory(TEXT("Config"), LOCTEXT("ChartConfig_DetailCatText", "Chart Config"));
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanChart, ChartTitle), UKantanChart::StaticClass());

	auto& PaddingGroup = ConfigCat.AddGroup(TEXT("Padding"), LOCTEXT("PaddingGroup", "Padding"));

	auto ChartPaddingProp = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UKantanChart, Margins), UKantanChart::StaticClass());
	PaddingGroup.AddPropertyRow(ChartPaddingProp).DisplayName(LOCTEXT("ChartPadding", "Chart"));
	auto TitlePaddingProp = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UKantanChart, TitlePadding), UKantanChart::StaticClass());
	PaddingGroup.AddPropertyRow(TitlePaddingProp).DisplayName(LOCTEXT("TitlePadding", "Title"));

	DetailBuilder.EditCategory(TEXT("Style"), LOCTEXT("ChartStyle_DetailCatText", "Chart Style"));
	DetailBuilder.EditCategory(TEXT("Category Styles"), LOCTEXT("ChartCategoryStyles_DetailCatText", "Category Styles"));
}


#undef LOCTEXT_NAMESPACE

