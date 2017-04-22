// Copyright (C) 2015-2017 Cameron Angus. All Rights Reserved.

#include "SeriesStyleManualMappingCustomization.h"
#include "PropertyEditing.h"
#include "KantanCartesianChartBase.h"

#define LOCTEXT_NAMESPACE "KantanChartsEditor"


TSharedRef< IPropertyTypeCustomization > FSeriesStyleManualMappingCustomization::MakeInstance()
{
	return MakeShareable(new FSeriesStyleManualMappingCustomization);
}

void FSeriesStyleManualMappingCustomization::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	HeaderRow.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget()
		];
}

void FSeriesStyleManualMappingCustomization::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr< IPropertyHandle > PropCatId = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSeriesStyleManualMapping, SeriesId));
	StructBuilder.AddChildProperty(PropCatId.ToSharedRef());

	auto SeriesStyleStruct = FKantanSeriesStyle::StaticStruct();
	TSharedPtr< IPropertyHandle > PropStyle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSeriesStyleManualMapping, Style));
	for (TFieldIterator< UProperty > It(SeriesStyleStruct); It; ++It)
	{
		auto ChildProp = *It;
		auto ChildPropName = ChildProp->GetFName();
		if (ChildPropName != GET_MEMBER_NAME_CHECKED(FKantanSeriesStyle, StyleId))
		{
			TSharedPtr< IPropertyHandle > PropChild = PropStyle->GetChildHandle(ChildPropName);
			StructBuilder.AddChildProperty(PropChild.ToSharedRef());
		}
	}
}


#undef LOCTEXT_NAMESPACE


