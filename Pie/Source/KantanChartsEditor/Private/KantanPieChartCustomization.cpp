// Fill out your copyright notice in the Description page of Project Settings.

#include "KantanChartsEditor.h"
#include "KantanPieChartCustomization.h"
#include "PropertyEditing.h"
#include "KantanPieChartBase.h"
#include "DetailHelpers.h"

#define LOCTEXT_NAMESPACE "KantanChartsEditor"


TSharedRef< IDetailCustomization > FKantanPieChartCustomization::MakeInstance()
{
	return MakeShareable(new FKantanPieChartCustomization);
}

void FKantanPieChartCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	auto& PreviewCat = DetailBuilder.EditCategory(TEXT("Preview"), LOCTEXT("ChartPreview_DetailCatText", "Preview"));
	PreviewCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanChart, bShowDataPreview), UKantanChart::StaticClass());

	auto& ConfigCat = DetailBuilder.EditCategory(TEXT("Config"), LOCTEXT("ChartConfig_DetailCatText", "Chart Config"));
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanChart, ChartTitle), UKantanChart::StaticClass());
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanChart, UpdateTickRate), UKantanChart::StaticClass());

	// Since we are using the same customization for both standard (with datasource property) and simple
	// (integrated datasource) charts, we just make an assumption about the property name, and do nothing
	// if it's not found.
	auto DatasourceProp = DetailBuilder.GetProperty(TEXT("Datasource"));
	if (DatasourceProp->IsValidHandle())
	{
		auto DatasourceComboButton = DetailHelpers::MakeInterfaceAssetPicker(DatasourceProp, UKantanCategoryDatasourceInterface::StaticClass());

		ConfigCat.AddCustomRow(LOCTEXT("DatasourceFilter", "Datasource"), true)
			.NameContent()
			[
				DatasourceProp->CreatePropertyNameWidget()
			]
		.ValueContent()
			[
				DatasourceComboButton
			]
		;

		DetailBuilder.HideProperty(TEXT("Datasource"));
	}

	auto& PaddingGroup = ConfigCat.AddGroup(TEXT("Padding"), LOCTEXT("PaddingGroup", "Padding"));

	auto ChartPaddingProp = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UKantanChart, Margins), UKantanChart::StaticClass());
	PaddingGroup.AddPropertyRow(ChartPaddingProp).DisplayName(LOCTEXT("ChartPadding", "Chart"));
	auto TitlePaddingProp = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UKantanChart, TitlePadding), UKantanChart::StaticClass());
	PaddingGroup.AddPropertyRow(TitlePaddingProp).DisplayName(LOCTEXT("TitlePadding", "Title"));

	DetailBuilder.EditCategory(TEXT("Style"), LOCTEXT("ChartStyle_DetailCatText", "Chart Style"));
	DetailBuilder.EditCategory(TEXT("Category Styles"), LOCTEXT("ChartCategoryStyles_DetailCatText", "Category Styles"));
}


#undef LOCTEXT_NAMESPACE

