// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "KantanBarChartCustomization.h"
#include "KantanBarChartBase.h"
#include "DetailHelpers.h"

#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailGroup.h"

#define LOCTEXT_NAMESPACE "KantanChartsEditor"


TSharedRef< IDetailCustomization > FKantanBarChartCustomization::MakeInstance()
{
	return MakeShareable(new FKantanBarChartCustomization);
}

void FKantanBarChartCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	auto& PreviewCat = DetailBuilder.EditCategory(TEXT("Preview"), LOCTEXT("ChartPreview_DetailCatText", "Preview"));
	PreviewCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanChart, bShowDataPreview), UKantanChart::StaticClass());

	auto& ConfigCat = DetailBuilder.EditCategory(TEXT("Config"), LOCTEXT("ChartConfig_DetailCatText", "Chart Config"));
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanChart, ChartTitle), UKantanChart::StaticClass());
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanBarChartBase, Orientation), UKantanBarChartBase::StaticClass());
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanBarChartBase, LabelPosition), UKantanBarChartBase::StaticClass());
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanBarChartBase, BarToGapRatio), UKantanBarChartBase::StaticClass());
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanBarChartBase, MaxBarValue), UKantanBarChartBase::StaticClass());
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanBarChartBase, ValueAxisCfg), UKantanBarChartBase::StaticClass());
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanBarChartBase, ValueExtentsDisplay), UKantanBarChartBase::StaticClass());
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

