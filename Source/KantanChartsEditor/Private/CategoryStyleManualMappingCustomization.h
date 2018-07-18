// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "IPropertyTypeCustomization.h"


class FCategoryStyleManualMappingCustomization : public IPropertyTypeCustomization
{
public:
	// Begin IPropertyTypeCustomization interface
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	// End IPropertyTypeCustomization interface

	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
};



