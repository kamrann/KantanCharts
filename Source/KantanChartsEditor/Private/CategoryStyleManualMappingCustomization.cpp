// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "CategoryStyleManualMappingCustomization.h"
#include "KantanCategoryChart.h"
#include "Style/KantanCategoryStyle.h"

#include "PropertyHandle.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"

#define LOCTEXT_NAMESPACE "KantanChartsEditor"


TSharedRef< IPropertyTypeCustomization > FCategoryStyleManualMappingCustomization::MakeInstance()
{
	return MakeShareable(new FCategoryStyleManualMappingCustomization);
}

void FCategoryStyleManualMappingCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	HeaderRow.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget()
		];
}

void FCategoryStyleManualMappingCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr< IPropertyHandle > PropCatId = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCategoryStyleManualMapping, CategoryId));
	StructBuilder.AddProperty(PropCatId.ToSharedRef());

	auto CatStyleStruct = FKantanCategoryStyle::StaticStruct();
	TSharedPtr< IPropertyHandle > PropStyle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCategoryStyleManualMapping, Style));
	for (TFieldIterator< UProperty > It(CatStyleStruct); It; ++It)
	{
		auto ChildProp = *It;
		auto ChildPropName = ChildProp->GetFName();
		if (ChildPropName != GET_MEMBER_NAME_CHECKED(FKantanCategoryStyle, CategoryStyleId))
		{
			TSharedPtr< IPropertyHandle > PropChild = PropStyle->GetChildHandle(ChildPropName);
			StructBuilder.AddProperty(PropChild.ToSharedRef());
		}
	}
}


#undef LOCTEXT_NAMESPACE


