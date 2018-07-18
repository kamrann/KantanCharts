// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "KantanBarChartCustomization.h"
#include "KantanCartesianPlotCustomization.h"
#include "KantanCartesianPlotScaleCustomization.h"
#include "KantanTimeSeriesPlotCustomization.h"
#include "CartesianAxisConfigCustomization.h"
#include "CategoryStyleManualMappingCustomization.h"
#include "SeriesStyleManualMappingCustomization.h"

//
#include "KantanSimpleCategoryDatasource.h"
#include "KantanSimpleCartesianDatasource.h"
//

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "ClassIconFinder.h"

#include "KCIKantanPluginInstallation.h"


const FString PluginName = TEXT("KantanCharts");

class FKantanChartsEditorModule : public FDefaultModuleImpl
{
	virtual void StartupModule() override
	{
		// Register detail customizations
		{
			auto& PropertyModule = FModuleManager::LoadModuleChecked< FPropertyEditorModule >("PropertyEditor");

			PropertyModule.RegisterCustomClassLayout(
				"BarChart",
				FOnGetDetailCustomizationInstance::CreateStatic(&FKantanBarChartCustomization::MakeInstance)
				);
			PropertyModule.RegisterCustomClassLayout(
				"SimpleBarChart",
				FOnGetDetailCustomizationInstance::CreateStatic(&FKantanBarChartCustomization::MakeInstance)
				);
			PropertyModule.RegisterCustomClassLayout(
				"CartesianPlot",
				FOnGetDetailCustomizationInstance::CreateStatic(&FKantanCartesianPlotCustomization::MakeInstance)
				);
			PropertyModule.RegisterCustomClassLayout(
				"SimpleCartesianPlot",
				FOnGetDetailCustomizationInstance::CreateStatic(&FKantanCartesianPlotCustomization::MakeInstance)
				);
			PropertyModule.RegisterCustomClassLayout(
				"TimeSeriesPlot",
				FOnGetDetailCustomizationInstance::CreateStatic(&FKantanTimeSeriesPlotCustomization::MakeInstance)
				);
			PropertyModule.RegisterCustomClassLayout(
				"SimpleTimeSeriesPlot",
				FOnGetDetailCustomizationInstance::CreateStatic(&FKantanTimeSeriesPlotCustomization::MakeInstance)
				);

			PropertyModule.RegisterCustomPropertyTypeLayout(
				"KantanCartesianPlotScale",
				FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FKantanCartesianPlotScaleCustomization::MakeInstance)
				);
			PropertyModule.RegisterCustomPropertyTypeLayout(
				"CartesianAxisRange",
				FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCartesianAxisRangeCustomization::MakeInstance)
				);
			PropertyModule.RegisterCustomPropertyTypeLayout(
				"CartesianAxisInstanceConfig",
				FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCartesianAxisInstanceConfigCustomization::MakeInstance)
				);
			PropertyModule.RegisterCustomPropertyTypeLayout(
				"CartesianAxisConfig",
				FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCartesianAxisConfigCustomization::MakeInstance)
				);
			PropertyModule.RegisterCustomPropertyTypeLayout(
				"CartesianRangeBound",
				FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCartesianRangeBoundCustomization::MakeInstance)
				);
			PropertyModule.RegisterCustomPropertyTypeLayout(
				"CategoryStyleManualMapping",
				FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCategoryStyleManualMappingCustomization::MakeInstance)
				);
			PropertyModule.RegisterCustomPropertyTypeLayout(
				"SeriesStyleManualMapping",
				FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FSeriesStyleManualMappingCustomization::MakeInstance)
				);

			PropertyModule.NotifyCustomizationModuleChanged();
		}

		// Chart preview data
		CreatePreviewDatasources();

		KCKantanInstallation::InitializeKantanPluginInstallation(PluginName, false);
	}

	virtual void ShutdownModule() override
	{
		KCKantanInstallation::ShutdownKantanPluginInstallation();

		// Unregister customizations
		if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
		{
			auto& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
			
			PropertyModule.UnregisterCustomClassLayout("BarChart");
			PropertyModule.UnregisterCustomClassLayout("SimpleBarChart");
			PropertyModule.UnregisterCustomClassLayout("CartesianPlot");
			PropertyModule.UnregisterCustomClassLayout("SimpleCartesianPlot");
			PropertyModule.UnregisterCustomClassLayout("TimeSeriesPlot");
			PropertyModule.UnregisterCustomClassLayout("SimpleTimeSeriesPlot");

			PropertyModule.UnregisterCustomPropertyTypeLayout("KantanCartesianPlotScale");
			PropertyModule.UnregisterCustomPropertyTypeLayout("CartesianAxisRange");
			PropertyModule.UnregisterCustomPropertyTypeLayout("CartesianAxisInstanceConfig");
			PropertyModule.UnregisterCustomPropertyTypeLayout("CartesianAxisConfig");
			PropertyModule.UnregisterCustomPropertyTypeLayout("CartesianRangeBound");
			PropertyModule.UnregisterCustomPropertyTypeLayout("CategoryStyleManualMapping");
			PropertyModule.UnregisterCustomPropertyTypeLayout("SeriesStyleManualMapping");
		}

		if (PreviewCategoryDatasource.IsValid())
		{
			PreviewCategoryDatasource->RemoveFromRoot();
			PreviewCategoryDatasource = nullptr;
		}

		if (PreviewCartesianPlotDatasource.IsValid())
		{
			PreviewCartesianPlotDatasource->RemoveFromRoot();
			PreviewCartesianPlotDatasource = nullptr;
		}

		if (PreviewTimeSeriesPlotDatasource.IsValid())
		{
			PreviewTimeSeriesPlotDatasource->RemoveFromRoot();
			PreviewTimeSeriesPlotDatasource = nullptr;
		}
	}

	void CreatePreviewDatasources()
	{
		if (ensure(PreviewCategoryDatasource == nullptr))
		{
			PreviewCategoryDatasource = NewObject< UKantanSimpleCategoryDatasource >(
				GetTransientPackage(),
				TEXT("PreviewCategoryDatasource"),
				RF_Transient
				);

			{
				auto Id = PreviewCategoryDatasource->AddCategory(FText::FromString(TEXT("Cat A")));
				PreviewCategoryDatasource->UpdateCategoryValue(Id, 0.5f);
			}

			{
				auto Id = PreviewCategoryDatasource->AddCategory(FText::FromString(TEXT("Cat B")));
				PreviewCategoryDatasource->UpdateCategoryValue(Id, 0.8f);
			}

			{
				auto Id = PreviewCategoryDatasource->AddCategory(FText::FromString(TEXT("Long Category C")));
				PreviewCategoryDatasource->UpdateCategoryValue(Id, 0.2f);
			}

			PreviewCategoryDatasource->AddToRoot();
		}

		if (ensure(PreviewCartesianPlotDatasource == nullptr))
		{
			PreviewCartesianPlotDatasource = NewObject< UKantanSimpleCartesianDatasource >(
				GetTransientPackage(),
				TEXT("PreviewCartesianPlotDatasource"),
				RF_Transient
				);

			{
				auto Id = PreviewCartesianPlotDatasource->AddSeries(FText::FromString(TEXT("Series 1")));
				PreviewCartesianPlotDatasource->AddDatapoint(Id, FVector2D(50.0f, 50.0f));
				PreviewCartesianPlotDatasource->AddDatapoint(Id, FVector2D(100.0f, 0.0f));
			}

			{
				auto Id = PreviewCartesianPlotDatasource->AddSeries(FText::FromString(TEXT("Series 2")));
				PreviewCartesianPlotDatasource->AddDatapoint(Id, FVector2D(0.0f, 0.0f));
				PreviewCartesianPlotDatasource->AddDatapoint(Id, FVector2D(0.0f, -200.0f));
				PreviewCartesianPlotDatasource->AddDatapoint(Id, FVector2D(-100.0f, -250.0f));
			}

			PreviewCartesianPlotDatasource->AddToRoot();
		}

		if (ensure(PreviewTimeSeriesPlotDatasource == nullptr))
		{
			PreviewTimeSeriesPlotDatasource = NewObject< UKantanSimpleCartesianDatasource >(
				GetTransientPackage(),
				TEXT("PreviewTimeSeriesPlotDatasource"),
				RF_Transient
				);

			{
				auto Id = PreviewTimeSeriesPlotDatasource->AddSeries(FText::FromString(TEXT("Series 1")));
				PreviewTimeSeriesPlotDatasource->AddDatapoint(Id, FVector2D(50.0f, 50.0f));
				PreviewTimeSeriesPlotDatasource->AddDatapoint(Id, FVector2D(100.0f, 0.0f));
			}

			{
				auto Id = PreviewTimeSeriesPlotDatasource->AddSeries(FText::FromString(TEXT("Series 2")));
				PreviewTimeSeriesPlotDatasource->AddDatapoint(Id, FVector2D(0.0f, 0.0f));
				PreviewTimeSeriesPlotDatasource->AddDatapoint(Id, FVector2D(20.0f, 10.0f));
				PreviewTimeSeriesPlotDatasource->AddDatapoint(Id, FVector2D(40.0f, 40.0f));
				PreviewTimeSeriesPlotDatasource->AddDatapoint(Id, FVector2D(60.0f, 100.0f));
			}

			PreviewTimeSeriesPlotDatasource->AddToRoot();
		}
	}

	// @NOTE: Using weak pointers since even though these get added to the root, it appears during shutdown process they can get released before
	// our own module shuts down. Weak pointer nulling still seems to work however.
	TWeakObjectPtr< UKantanSimpleCategoryDatasource > PreviewCategoryDatasource;
	TWeakObjectPtr< UKantanSimpleCartesianDatasource > PreviewCartesianPlotDatasource;
	TWeakObjectPtr< UKantanSimpleCartesianDatasource > PreviewTimeSeriesPlotDatasource;

public:
	FKantanChartsEditorModule() :
		PreviewCategoryDatasource(nullptr)
		, PreviewCartesianPlotDatasource(nullptr)
		, PreviewTimeSeriesPlotDatasource(nullptr)
	{}
};


IMPLEMENT_MODULE(FKantanChartsEditorModule, KantanChartsEditor);

