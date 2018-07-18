// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "KantanCartesianPlotScaleCustomization.h"
#include "KantanCartesianTypes.h"

#include "PropertyCustomizationHelpers.h"
#include "DetailLayoutBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SNumericEntryBox.h"

#define LOCTEXT_NAMESPACE "KantanChartsEditor"


TSharedRef< IPropertyTypeCustomization > FKantanCartesianPlotScaleCustomization::MakeInstance()
{
	return MakeShareable(new FKantanCartesianPlotScaleCustomization);
}

void FKantanCartesianPlotScaleCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	static const FName ScalingTypePropertyName = GET_MEMBER_NAME_CHECKED(FKantanCartesianPlotScale, Type);

	const bool bDisplayResetToDefault = false;
	const FText DisplayNameOverride = FText::GetEmpty();
	const FText DisplayToolTipOverride = FText::GetEmpty();

	// Grab property handle for scaling type
	ScalingTypeProp = StructPropertyHandle->GetChildHandle(ScalingTypePropertyName);
	check(ScalingTypeProp.IsValid());

	// Register handler for scaling type change
	ScalingTypeProp->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FKantanCartesianPlotScaleCustomization::OnScalingTypeChanged));

	// Cache the initial scaling type
	OnScalingTypeChanged();

	TSharedPtr< SVerticalBox > VerticalBox;

	HeaderRow
		.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget(DisplayNameOverride, DisplayToolTipOverride, bDisplayResetToDefault)
		]
		.ValueContent()
		[
			SAssignNew(VerticalBox, SVerticalBox)
		];
	
	VerticalBox->AddSlot()
		[
			SNew(SProperty, ScalingTypeProp)
			.ShouldDisplayName(false)
		];
}

void FKantanCartesianPlotScaleCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	{
		auto& Row = StructBuilder.AddProperty(StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FKantanCartesianPlotScale, Scale)).ToSharedRef());
		Row.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FKantanCartesianPlotScaleCustomization::GetFixedScaleAndFocusVisibility)));
	}

	{
		auto& Row = StructBuilder.AddProperty(StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FKantanCartesianPlotScale, FocalCoordinates)).ToSharedRef());
		Row.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FKantanCartesianPlotScaleCustomization::GetFixedScaleAndFocusVisibility)));
	}

	{
		auto& Row = StructBuilder.AddProperty(StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FKantanCartesianPlotScale, RangeX)).ToSharedRef());
		Row.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FKantanCartesianPlotScaleCustomization::GetFixedRangeVisibility)));
	}

	{
		auto& Row = StructBuilder.AddProperty(StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FKantanCartesianPlotScale, RangeY)).ToSharedRef());
		Row.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FKantanCartesianPlotScaleCustomization::GetFixedRangeVisibility)));
	}
}

void FKantanCartesianPlotScaleCustomization::OnScalingTypeChanged()
{
	uint8 UintVal;
	ScalingTypeProp->GetValue(UintVal);
	CurrentScalingType = static_cast< ECartesianScalingType >(UintVal);
}

EVisibility FKantanCartesianPlotScaleCustomization::GetFixedScaleAndFocusVisibility() const
{
	return CurrentScalingType == ECartesianScalingType::FixedScale ? EVisibility::Visible : EVisibility::Hidden;
}

EVisibility FKantanCartesianPlotScaleCustomization::GetFixedRangeVisibility() const
{
	return CurrentScalingType == ECartesianScalingType::FixedRange ? EVisibility::Visible : EVisibility::Hidden;
}


TSharedRef< IPropertyTypeCustomization > FCartesianAxisRangeCustomization::MakeInstance()
{
	return MakeShareable(new FCartesianAxisRangeCustomization);
}

void FCartesianAxisRangeCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	static const FName RangeMinPropertyName = GET_MEMBER_NAME_CHECKED(FCartesianAxisRange, Min);
	static const FName RangeMaxPropertyName = GET_MEMBER_NAME_CHECKED(FCartesianAxisRange, Max);

	const bool bDisplayResetToDefault = false;
	const FText DisplayNameOverride = FText::GetEmpty();
	const FText DisplayToolTipOverride = FText::GetEmpty();

	MinProp = StructPropertyHandle->GetChildHandle(RangeMinPropertyName);
	MaxProp = StructPropertyHandle->GetChildHandle(RangeMaxPropertyName);

	TSharedPtr< SHorizontalBox > HorizontalBox;

	HeaderRow
		.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget(DisplayNameOverride, DisplayToolTipOverride, bDisplayResetToDefault)
		]
		.ValueContent()
		.MinDesiredWidth(125.0f * 2)
		.MaxDesiredWidth(125.0f * 2)
		[
			SAssignNew(HorizontalBox, SHorizontalBox)
		];

	auto MinPropRef = MinProp.ToSharedRef();
	HorizontalBox->AddSlot()
		.Padding(FMargin(0.0f, 2.0f, 3.0f, 2.0f))
		[
			MakeComponentWidget(StructPropertyHandle, MinPropRef)
		];
	auto MaxPropRef = MaxProp.ToSharedRef();
	HorizontalBox->AddSlot()
		.Padding(FMargin(0.0f, 2.0f, 0.0f, 2.0f))
		[
			MakeComponentWidget(StructPropertyHandle, MaxPropRef)
		];
}

void FCartesianAxisRangeCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	
}

TOptional<float> FCartesianAxisRangeCustomization::OnGetValue(TWeakPtr<IPropertyHandle> WeakHandlePtr) const
{
	float NumericVal = 0;
	if (WeakHandlePtr.Pin()->GetValue(NumericVal) == FPropertyAccess::Success)
	{
		return TOptional<float>(NumericVal);
	}

	// Value couldn't be accessed.  Return an unset value
	return TOptional<float>();
}

void FCartesianAxisRangeCustomization::OnValueCommitted(float NewValue, ETextCommit::Type CommitType, TWeakPtr<IPropertyHandle> WeakHandlePtr)
{
	EPropertyValueSetFlags::Type Flags = EPropertyValueSetFlags::DefaultFlags;
	SetValue(NewValue, Flags, WeakHandlePtr);
}

void FCartesianAxisRangeCustomization::OnValueChanged(float NewValue, TWeakPtr<IPropertyHandle> WeakHandlePtr)
{
	if (false) // @TODO: bIsUsingSlider)
	{
		EPropertyValueSetFlags::Type Flags = EPropertyValueSetFlags::InteractiveChange;
		SetValue(NewValue, Flags, WeakHandlePtr);
	}
}

void FCartesianAxisRangeCustomization::SetValue(float NewValue, EPropertyValueSetFlags::Type Flags, TWeakPtr<IPropertyHandle> WeakHandlePtr)
{
	WeakHandlePtr.Pin()->SetValue(NewValue, Flags);
}

TSharedRef< SWidget > FCartesianAxisRangeCustomization::MakeComponentWidget(
	TSharedRef<IPropertyHandle>& StructPropertyHandle,
	TSharedRef<IPropertyHandle>& PropertyHandle
	)
{
	TWeakPtr<IPropertyHandle> WeakHandlePtr = PropertyHandle;

	return SNew(SNumericEntryBox< float >)
		//			.IsEnabled(this, &FMathStructCustomization::IsValueEnabled, WeakHandlePtr)
		.Value(this, &FCartesianAxisRangeCustomization::OnGetValue, WeakHandlePtr)
		.Font(IDetailLayoutBuilder::GetDetailFont())
		//			.UndeterminedString(NSLOCTEXT("PropertyEditor", "MultipleValues", "Multiple Values"))
		.OnValueCommitted(this, &FCartesianAxisRangeCustomization::OnValueCommitted, WeakHandlePtr)
		.OnValueChanged(this, &FCartesianAxisRangeCustomization::OnValueChanged, WeakHandlePtr)
		//			.OnBeginSliderMovement(this, &FMathStructCustomization::OnBeginSliderMovement)
		//			.OnEndSliderMovement(this, &FMathStructCustomization::OnEndSliderMovement<NumericType>)
		.LabelVAlign(VAlign_Center)
		// Only allow spin on handles with one object.  Otherwise it is not clear what value to spin
		.AllowSpin(PropertyHandle->GetNumOuterObjects() == 1)
		//			.MinValue(MinValue)
		//			.MaxValue(MaxValue)
		//			.MinSliderValue(SliderMinValue)
		//			.MaxSliderValue(SliderMaxValue)
		//			.SliderExponent(SliderExponent)
		//			.Delta(Delta)
		.Label()
		[
			SNew(STextBlock)
			.Font(IDetailLayoutBuilder::GetDetailFont())
			.Text(PropertyHandle->GetPropertyDisplayName())
		];
}


#undef LOCTEXT_NAMESPACE


