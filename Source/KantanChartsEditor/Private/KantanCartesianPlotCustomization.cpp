// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "KantanCartesianPlotCustomization.h"
#include "KantanCartesianPlotBase.h"
#include "DetailHelpers.h"

#include "PropertyHandle.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailGroup.h"

#define LOCTEXT_NAMESPACE "KantanChartsEditor"


TSharedRef< IDetailCustomization > FKantanCartesianPlotCustomization::MakeInstance()
{
	return MakeShareable(new FKantanCartesianPlotCustomization);
}

void FKantanCartesianPlotCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	static const FName PlotScalePropertyName = GET_MEMBER_NAME_CHECKED(UKantanCartesianChartBase, PlotScale);

	// Grab property handle for plot scale
	PlotScaleProp = DetailBuilder.GetProperty(PlotScalePropertyName);
	check(PlotScaleProp.IsValid());

	// Register change handler
	PlotScaleProp->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FKantanCartesianPlotCustomization::OnValidatePlotScale));

	auto& PreviewCat = DetailBuilder.EditCategory(TEXT("Preview"), LOCTEXT("ChartPreview_DetailCatText", "Preview"));
	PreviewCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanChart, bShowDataPreview), UKantanChart::StaticClass());

	auto& ConfigCat = DetailBuilder.EditCategory(TEXT("Config"), LOCTEXT("ChartConfig_DetailCatText", "Chart Config"));

	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanChart, ChartTitle), UKantanChart::StaticClass());
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanCartesianChartBase, XAxisCfg), UKantanCartesianChartBase::StaticClass());
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanCartesianChartBase, YAxisCfg), UKantanCartesianChartBase::StaticClass());
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanCartesianChartBase, PlotScale), UKantanCartesianChartBase::StaticClass());
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanCartesianChartBase, DataPointSize), UKantanCartesianChartBase::StaticClass());
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanChart, UpdateTickRate), UKantanChart::StaticClass());

	// Since we are using the same customization for both standard (with datasource property) and simple
	// (integrated datasource) charts, we just make an assumption about the property name, and do nothing
	// if it's not found.
	auto DatasourceProp = DetailBuilder.GetProperty(TEXT("Datasource"));
	if (DatasourceProp->IsValidHandle())
	{
		auto DatasourceComboButton = DetailHelpers::MakeInterfaceAssetPicker(DatasourceProp, UKantanCartesianDatasourceInterface::StaticClass());

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
	auto AxisTitlePaddingProp = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UKantanCartesianChartBase, AxisTitlePadding), UKantanCartesianChartBase::StaticClass());
	PaddingGroup.AddPropertyRow(AxisTitlePaddingProp).DisplayName(LOCTEXT("AxisTitlePadding", "Axis Title"));

	DetailBuilder.EditCategory(TEXT("Style"), LOCTEXT("ChartStyle_DetailCatText", "Chart Style"));
	DetailBuilder.EditCategory(TEXT("Series Styles"), LOCTEXT("ChartSeriesStyles_DetailCatText", "Series Styles"));
}

void FKantanCartesianPlotCustomization::OnValidatePlotScale()
{

}


#undef LOCTEXT_NAMESPACE

