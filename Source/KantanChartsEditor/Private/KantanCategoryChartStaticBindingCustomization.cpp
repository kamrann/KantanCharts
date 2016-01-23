// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

#include "KantanChartsEditor.h"

#if 0

#include "KantanCategoryChartStaticBindingCustomization.h"
#include "PropertyEditing.h"
#include "KantanCategoryChart.h"
//#include "SPropertyBinding.h"

#define LOCTEXT_NAMESPACE "KantanChartsEditor"


TSharedRef< IPropertyTypeCustomization > FKantanCategoryChartStaticBindingCustomization::MakeInstance()
{
	return MakeShareable(new FKantanCategoryChartStaticBindingCustomization);
}

void FKantanCategoryChartStaticBindingCustomization::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	HeaderRow.NameContent()
		[
			SNew(STextBlock).Text(FText::FromString(TEXT("Hello!!!")))
		];
}

void FKantanCategoryChartStaticBindingCustomization::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
/*	StructBuilder.AddChildContent(LOCTEXT("Test", "Test")).NameContent()
		[
			SNew(STextBlock).Text(FText::FromString(TEXT("Hello from child!")))
		];
		*/

	TSharedPtr<IPropertyHandle> PropCatLabel = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FStaticCategoryBinding, CategoryLabel));
	StructBuilder.AddChildProperty(PropCatLabel.ToSharedRef());
//		.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FEnvTraceDataCustomization::GetNavigationVisibility)));

	TSharedPtr<IPropertyHandle> PropValueBinding = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FStaticCategoryBinding, ValueBinding));
//	StructBuilder.AddChildProperty(PropValueBinding.ToSharedRef());

	auto DelegateProperty = Cast< UDelegateProperty >(PropValueBinding->GetProperty());

	StructBuilder.AddChildContent(LOCTEXT("ValueBinding", "Value Binding"))
		.NameContent()
		[
			SNew(STextBlock).Text(LOCTEXT("ValueBinding", "Value Binding"))
		]
/*		.ValueContent()
		[
			SNew(SPropertyBinding, ?????????????? BlueprintEditor.Pin().ToSharedRef(), DelegateProperty, PropValueBinding.ToSharedRef())
			.GeneratePureBindings(true)
		]
*/		;
}


#undef LOCTEXT_NAMESPACE

#endif

