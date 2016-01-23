// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

#pragma once

#if 0

#include "PropertyEditing.h"
#include "PropertyCustomizationHelpers.h"
//#include "KantanCategoryChart.h"


class FKantanCategoryChartStaticBindingCustomization: public IPropertyTypeCustomization
{
public:
	// Begin IPropertyTypeCustomization interface
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	// End IPropertyTypeCustomization interface

	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

protected:
//	FText GetProbabilitySumText() const;
//	EVisibility GetSelectedTrackedObjectVisibility() const;

private:
	// Keep a pointer for refreshing layout
	IDetailLayoutBuilder* DetailBuilderPtr;

//	class UObjectAwarenessComponent* Component;
//	TArray< TSharedPtr< TrackedEntityId > > TrackedList;
//	TSharedPtr< SComboBox< TSharedPtr< TrackedEntityId > > > TrackedCombo;
//	class AActor* CurrentTracked;
//	TSharedPtr< class IDetailsView > LPD_Details;
};


#endif
