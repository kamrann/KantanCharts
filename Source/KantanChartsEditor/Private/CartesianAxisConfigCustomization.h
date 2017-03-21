// Copyright (C) 2015-2017 Cameron Angus. All Rights Reserved.

#pragma once

#include "PropertyEditing.h"
#include "PropertyCustomizationHelpers.h"
#include "CartesianAxisConfig.h"


class FCartesianAxisInstanceConfigCustomization: public IPropertyTypeCustomization
{
public:
	// Begin IPropertyTypeCustomization interface
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	// End IPropertyTypeCustomization interface

	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

protected:
//	TSharedPtr< IPropertyHandle > ScalingTypeProp;
};


class FCartesianAxisConfigCustomization : public IPropertyTypeCustomization
{
public:
	// Begin IPropertyTypeCustomization interface
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	// End IPropertyTypeCustomization interface

	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
};


class FCartesianRangeBoundCustomization : public IPropertyTypeCustomization
{
public:
	// Begin IPropertyTypeCustomization interface
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	// End IPropertyTypeCustomization interface

	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
};



