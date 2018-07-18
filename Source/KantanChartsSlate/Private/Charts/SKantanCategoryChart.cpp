// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "Charts/SKantanCategoryChart.h"
#include "KantanCategoryDatasourceInterface.h"
#include "Style/KantanCategoryStyleSet.h"


void SKantanCategoryChart::Construct(const FArguments& InArgs)
{
	SKantanChart::Construct(
		SKantanChart::FArguments()
		.UpdateTickRate(InArgs._UpdateTickRate)
		);

	SetDatasource(InArgs._Datasource);
	SetUseAutoPerCategoryStyles(true);
}

bool SKantanCategoryChart::SetDatasource(UObject* InDatasource)
{
	if (IsNullOrValidDatasource(InDatasource) == false)
	{
		return false;
	}

	if (Datasource != InDatasource)
	{
		Datasource = InDatasource;

		if (Datasource)
		{
			// Immediately update the data snapshot
			DataSnapshot.UpdateFromDatasource(Datasource);
		}
		else
		{
			DataSnapshot.Clear();
		}

		if (bAutoPerCategoryStyles)
		{
			UpdateDynamicCategoryStyleMappings();
		}
	}

	return true;
}

void SKantanCategoryChart::SetUseAutoPerCategoryStyles(bool bEnable)
{
	bAutoPerCategoryStyles = bEnable;
}

void SKantanCategoryChart::SetCategoryStylesList(TArray< FKantanCategoryStyle > const& Styles)
{
	CategoryStyles = Styles;
}

void SKantanCategoryChart::LoadCategoryStylesList(const FSoftObjectPath& Styles)
{
	auto CatStyleSet = Cast< UKantanCategoryStyleSet >(Styles.TryLoad());
	if(CatStyleSet)
	{
		SetCategoryStylesList(CatStyleSet->Styles);
	}
}

void SKantanCategoryChart::SetManualCategoryStyleMappings(TMap< FName, FName > const& Mappings)
{
	// @NOTE: Currently just overwriting so any mappings which were auto-assigned will get removed,
	// and will be reassigned on next tick.
	CategoryStyleMapping = Mappings;
}

void SKantanCategoryChart::ResetCategoryStyleMappings()
{
	CategoryStyleMapping.Empty();
}

int32 SKantanCategoryChart::GetNumCategories() const
{
	return DataSnapshot.Elements.Num();
}

FName SKantanCategoryChart::GetCategoryId(int32 Index) const
{
	return DataSnapshot.Elements[Index].Id;
}

FText SKantanCategoryChart::GetCategoryLabel(int32 Index) const
{
	return DataSnapshot.Elements[Index].Name;
}

float SKantanCategoryChart::GetCategoryValue(int32 Index) const
{
	return DataSnapshot.Elements[Index].Value;
}


FKantanCategoryStyle const& SKantanCategoryChart::GetCategoryStyle(FName CatId) const
{
	// @TODO: Expose as argument/property
	static FKantanCategoryStyle DefaultStyle = FKantanCategoryStyle();

	if (CatId.IsNone() == false)
	{
		auto MappingEntry = CategoryStyleMapping.Find(CatId);
		if (MappingEntry && MappingEntry->IsNone() == false)
		{
			auto Style = FindCategoryStyle(*MappingEntry);
			if (Style)
			{
				return *Style;
			}
		}
	}

	return DefaultStyle;
}

bool SKantanCategoryChart::IsValidDatasource(UObject* Source)
{
	return Source != nullptr &&
		Source->GetClass()->ImplementsInterface(IKantanCategoryDatasourceInterface::UClassType::StaticClass());
}

bool SKantanCategoryChart::IsNullOrValidDatasource(UObject* Source)
{
	return Source == nullptr || IsValidDatasource(Source);
}

void SKantanCategoryChart::UpdateDynamicCategoryStyleMappings()
{
	auto NumCats = GetNumCategories();
	for (int32 Idx = 0; Idx < NumCats; ++Idx)
	{
		auto CatId = GetCategoryId(Idx);
		if (CategoryStyleMapping.Contains(CatId) == false)
		{
			// This category currently has no style mapping
			CategoryStyleMapping.Add(CatId, GetNextCategoryStyle());
		}
	}

	// @TODO: Better to remove mappings for categories no longer in the data source, or leave them in?
}

FName SKantanCategoryChart::GetNextCategoryStyle() const
{
	for (auto const& Style : CategoryStyles)
	{
		if (CategoryStyleMapping.FindKey(Style.CategoryStyleId) == nullptr)
		{
			// This style currently not being used
			return Style.CategoryStyleId;
		}
	}

	// Currently if no unused styles available, return None (which will lead to using default style)
	return NAME_None;
}

/**
 * Computes the desired size of this widget (SWidget)
 *
 * @return  The widget's desired size
 */
FVector2D SKantanCategoryChart::ComputeDesiredSize( float ) const
{
	return FVector2D(300, 300);
}

void SKantanCategoryChart::OnActiveTick(double InCurrentTime, float InDeltaTime)
{
	if (Datasource != nullptr)
	{
		DataSnapshot.UpdateFromDatasource(Datasource);

		if (bAutoPerCategoryStyles)
		{
			UpdateDynamicCategoryStyleMappings();
		}
	}
}

void SKantanCategoryChart::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SKantanChart::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

void SKantanCategoryChart::AddReferencedObjects(FReferenceCollector& Collector)
{
	if (Datasource)
	{
		Collector.AddReferencedObject(Datasource);
	}
}



