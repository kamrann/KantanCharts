// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "KantanTimeSeriesPlotCustomization.h"
#include "KantanTimeSeriesPlotBase.h"
#include "DetailHelpers.h"

#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "IDetailGroup.h"
#include "DetailWidgetRow.h"

#define LOCTEXT_NAMESPACE "KantanChartsEditor"


TSharedRef< IDetailCustomization > FKantanTimeSeriesPlotCustomization::MakeInstance()
{
	return MakeShareable(new FKantanTimeSeriesPlotCustomization);
}

void FKantanTimeSeriesPlotCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	auto& PreviewCat = DetailBuilder.EditCategory(TEXT("Preview"), LOCTEXT("ChartPreview_DetailCatText", "Preview"));
	PreviewCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanChart, bShowDataPreview), UKantanChart::StaticClass());

	auto& ConfigCat = DetailBuilder.EditCategory(TEXT("Config"), LOCTEXT("ChartConfig_DetailCatText", "Chart Config"));

	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanChart, ChartTitle), UKantanChart::StaticClass());
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanCartesianChartBase, XAxisCfg), UKantanCartesianChartBase::StaticClass());
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanTimeSeriesPlotBase, bUseFixedTimeRange), UKantanTimeSeriesPlotBase::StaticClass());
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanTimeSeriesPlotBase, DisplayTimeRange), UKantanTimeSeriesPlotBase::StaticClass());
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanTimeSeriesPlotBase, LowerTimeBound), UKantanTimeSeriesPlotBase::StaticClass());
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanTimeSeriesPlotBase, UpperTimeBound), UKantanTimeSeriesPlotBase::StaticClass());
	//ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanTimeSeriesPlotBase, bRoundTimeRange), UKantanTimeSeriesPlotBase::StaticClass());
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanCartesianChartBase, YAxisCfg), UKantanCartesianChartBase::StaticClass());
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanTimeSeriesPlotBase, LowerValueBound), UKantanTimeSeriesPlotBase::StaticClass());
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanTimeSeriesPlotBase, UpperValueBound), UKantanTimeSeriesPlotBase::StaticClass());
	DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(UKantanCartesianChartBase, PlotScale), UKantanCartesianChartBase::StaticClass());
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanCartesianChartBase, DataPointSize), UKantanCartesianChartBase::StaticClass());
	ConfigCat.AddProperty(GET_MEMBER_NAME_CHECKED(UKantanChart, UpdateTickRate), UKantanChart::StaticClass());

	auto& PaddingGroup = ConfigCat.AddGroup(TEXT("Padding"), LOCTEXT("PaddingGroup", "Padding"));
	
	auto ChartPaddingProp = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UKantanChart, Margins), UKantanChart::StaticClass());
	PaddingGroup.AddPropertyRow(ChartPaddingProp).DisplayName(LOCTEXT("ChartPadding", "Chart"));
	auto TitlePaddingProp = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UKantanChart, TitlePadding), UKantanChart::StaticClass());
	PaddingGroup.AddPropertyRow(TitlePaddingProp).DisplayName(LOCTEXT("TitlePadding", "Title"));
	auto AxisTitlePaddingProp = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UKantanCartesianChartBase, AxisTitlePadding), UKantanCartesianChartBase::StaticClass());
	PaddingGroup.AddPropertyRow(AxisTitlePaddingProp).DisplayName(LOCTEXT("AxisTitlePadding", "Axis Title"));

	// Since we are using the same customization for both standard (with datasource property) and simple
	// (integrated datasource) charts, we just make an assumption about the property name, and do nothing
	// if it's not found.
	auto DatasourceProp = DetailBuilder.GetProperty(TEXT("Datasource"));
	if (DatasourceProp->IsValidHandle())
	{
/*		auto OnDatasourceSelected = [DatasourceProp](const class FAssetData& AssetData)
		{
			if (DatasourceProp->IsValidHandle())
			{
				const UObject* Asset = AssetData.GetAsset();
				DatasourceProp->SetValue(Asset);
			}
		};

		auto IsDatasourceAsset = [](const class FAssetData& AssetData) -> bool
		{
			// @TODO: Calling GetAsset causes all incoming assets to be force-loaded if not already,
			// which is clearly not going to work. 
			// Can't get class if the asset is not loaded though, so can't determine if it implements an interface.
			// Currently only considering already loaded assets, alternative is to make a UObject base class for
			// static datasource assets.
			auto Cls = AssetData.GetClass();
			return Cls == nullptr || Cls->ImplementsInterface(UKantanCartesianDatasourceInterface::StaticClass()) == false;
		};

		auto OnGetMenuContent = [DatasourceProp, OnDatasourceSelected, IsDatasourceAsset]() -> TSharedRef< SWidget >
		{
			auto& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked< FContentBrowserModule >(TEXT("ContentBrowser"));

			UObject* InitialObject = nullptr;
			if (DatasourceProp->IsValidHandle())
			{
				DatasourceProp->GetValue(InitialObject);
			}

			FAssetPickerConfig AssetPickerConfig;
			// We want any valid object implementing the interface, so any child of UObject as the filter
			AssetPickerConfig.Filter.ClassNames.Add(UObject::StaticClass()->GetFName());
			// Allow child classes
			AssetPickerConfig.Filter.bRecursiveClasses = true;
			// Set a delegate for setting the asset from the picker
			AssetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateLambda(OnDatasourceSelected);
			// Use the list view by default
			AssetPickerConfig.InitialAssetViewType = EAssetViewType::List;
			// The initial selection should be the current value
			AssetPickerConfig.InitialAssetSelection = FAssetData(InitialObject);
			// We'll do clearing ourselves
			AssetPickerConfig.bAllowNullSelection = true;
			// Focus search box
			AssetPickerConfig.bFocusSearchBoxWhenOpened = true;
			// Apply custom filter
			AssetPickerConfig.OnShouldFilterAsset = FOnShouldFilterAsset::CreateLambda(IsDatasourceAsset);
			// Don't allow dragging
			AssetPickerConfig.bAllowDragging = false;
			// Save the settings into a special section for asset pickers for properties
			//AssetPickerConfig.SaveSettingsName = TEXT("AssetPropertyPicker");

			return SNew(SBox)
				.WidthOverride(300.0f)
				.HeightOverride(300.0f)
				[
					ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig)
				];
		};

		auto OnGetCurrentName = [DatasourceProp]() -> FText
		{
			FText Text = FText::GetEmpty();
			DatasourceProp->GetValueAsDisplayText(Text);
			return Text;
		};

		auto DatasourceComboButton = SNew(SComboButton)
			//		.ToolTipText(this, &SPropertyEditorAsset::OnGetToolTip)
			//		.ButtonStyle(FEditorStyle::Get(), "PropertyEditor.AssetComboStyle")
			//		.ForegroundColor(FEditorStyle::GetColor("PropertyEditor.AssetName.ColorAndOpacity"))
			.OnGetMenuContent_Lambda(OnGetMenuContent)
			//.OnMenuOpenChanged(this, &SPropertyEditorAsset::OnMenuOpenChanged)
			//.IsEnabled(IsEnabledAttribute)
			.ContentPadding(2.0f)
			.ButtonContent()
			[
				// Show the name of the asset or actor
				SNew(STextBlock)
				//.TextStyle(FEditorStyle::Get(), "PropertyEditor.AssetClass")
				//.Font(FEditorStyle::GetFontStyle(PropertyEditorConstants::PropertyFontStyle))
				.Text_Lambda(OnGetCurrentName)
			];
*/
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

	DetailBuilder.EditCategory(TEXT("Style"), LOCTEXT("ChartStyle_DetailCatText", "Chart Style"));
	DetailBuilder.EditCategory(TEXT("Series Styles"), LOCTEXT("ChartSeriesStyles_DetailCatText", "Series Styles"));
}


#undef LOCTEXT_NAMESPACE

