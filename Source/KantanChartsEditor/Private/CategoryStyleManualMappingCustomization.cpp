// Copyright (C) 2015-2017 Cameron Angus. All Rights Reserved.

#include "CategoryStyleManualMappingCustomization.h"
#include "PropertyEditing.h"
#include "KantanCategoryChart.h"

#define LOCTEXT_NAMESPACE "KantanChartsEditor"


TSharedRef< IPropertyTypeCustomization > FCategoryStyleManualMappingCustomization::MakeInstance()
{
	return MakeShareable(new FCategoryStyleManualMappingCustomization);
}

void FCategoryStyleManualMappingCustomization::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	HeaderRow.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget()
		];
}

void FCategoryStyleManualMappingCustomization::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr< IPropertyHandle > PropCatId = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCategoryStyleManualMapping, CategoryId));
	StructBuilder.AddChildProperty(PropCatId.ToSharedRef());

	auto CatStyleStruct = FKantanCategoryStyle::StaticStruct();
	TSharedPtr< IPropertyHandle > PropStyle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCategoryStyleManualMapping, Style));
	for (TFieldIterator< UProperty > It(CatStyleStruct); It; ++It)
	{
		auto ChildProp = *It;
		auto ChildPropName = ChildProp->GetFName();
		if (ChildPropName != GET_MEMBER_NAME_CHECKED(FKantanCategoryStyle, CategoryStyleId))
		{
			TSharedPtr< IPropertyHandle > PropChild = PropStyle->GetChildHandle(ChildPropName);
			StructBuilder.AddChildProperty(PropChild.ToSharedRef());
		}
	}
}


#undef LOCTEXT_NAMESPACE


