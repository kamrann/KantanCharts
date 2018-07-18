// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "KantanCartesianTypes.h"

#include "IPropertyTypeCustomization.h"
#include "Layout/Visibility.h"
#include "PropertyHandle.h"


class SWidget;


class FKantanCartesianPlotScaleCustomization: public IPropertyTypeCustomization
{
public:
	// Begin IPropertyTypeCustomization interface
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	// End IPropertyTypeCustomization interface

	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

protected:
	void OnScalingTypeChanged();

	EVisibility GetFixedScaleAndFocusVisibility() const;
	EVisibility GetFixedRangeVisibility() const;

protected:
	TSharedPtr< IPropertyHandle > ScalingTypeProp;
	ECartesianScalingType CurrentScalingType;
};


class FCartesianAxisRangeCustomization : public IPropertyTypeCustomization
{
public:
	// Begin IPropertyTypeCustomization interface
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	// End IPropertyTypeCustomization interface

	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

protected:
	TOptional<float> OnGetValue(TWeakPtr<class IPropertyHandle> WeakHandlePtr) const;
	void OnValueCommitted(float NewValue, ETextCommit::Type CommitType, TWeakPtr<class IPropertyHandle> WeakHandlePtr);
	void OnValueChanged(float NewValue, TWeakPtr<class IPropertyHandle> WeakHandlePtr);

	void SetValue(float NewValue, EPropertyValueSetFlags::Type Flags, TWeakPtr<class IPropertyHandle> WeakHandlePtr);

	TSharedRef< SWidget > MakeComponentWidget(TSharedRef<class IPropertyHandle>& StructPropertyHandle, TSharedRef<class IPropertyHandle>& PropertyHandle);

protected:
	TSharedPtr< class IPropertyHandle > MinProp;
	TSharedPtr< class IPropertyHandle > MaxProp;
};


