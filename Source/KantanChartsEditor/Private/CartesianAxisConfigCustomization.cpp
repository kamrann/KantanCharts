// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "CartesianAxisConfigCustomization.h"
#include "KantanCartesianTypes.h"
#include "Charts/CartesianAxisConfig.h"

#include "PropertyHandle.h"
#include "PropertyCustomizationHelpers.h"
#include "IDetailChildrenBuilder.h"
#include "Widgets/SBoxPanel.h"

#define LOCTEXT_NAMESPACE "KantanChartsEditor"


TSharedRef< IPropertyTypeCustomization > FCartesianAxisInstanceConfigCustomization::MakeInstance()
{
	return MakeShareable(new FCartesianAxisInstanceConfigCustomization);
}

void FCartesianAxisInstanceConfigCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	static const FName EnabledPropertyName = GET_MEMBER_NAME_CHECKED(FCartesianAxisInstanceConfig, bEnabled);
	static const FName TitlePropertyName = GET_MEMBER_NAME_CHECKED(FCartesianAxisInstanceConfig, bShowTitle);
	static const FName MarkersPropertyName = GET_MEMBER_NAME_CHECKED(FCartesianAxisInstanceConfig, bShowMarkers);
	static const FName LabelsPropertyName = GET_MEMBER_NAME_CHECKED(FCartesianAxisInstanceConfig, bShowLabels);

	const bool bDisplayResetToDefault = false;
	const FText DisplayNameOverride = FText::GetEmpty();
	const FText DisplayToolTipOverride = FText::GetEmpty();

	auto EnabledProp = StructPropertyHandle->GetChildHandle(EnabledPropertyName);
	auto TitleProp = StructPropertyHandle->GetChildHandle(TitlePropertyName);
	auto MarkersProp = StructPropertyHandle->GetChildHandle(MarkersPropertyName);
	auto LabelsProp = StructPropertyHandle->GetChildHandle(LabelsPropertyName);

	TSharedPtr< SVerticalBox > HeaderBox;
	TSharedPtr< SHorizontalBox > HeaderHorizBox;
	TSharedPtr< SVerticalBox > PropertiesBox;

	HeaderRow
		.NameContent()
		.VAlign(EVerticalAlignment::VAlign_Fill)
		[
			SAssignNew(HeaderBox, SVerticalBox)
		]
		.ValueContent()
		[
			SAssignNew(PropertiesBox, SVerticalBox)
			.Visibility_Lambda([EnabledProp]
			{
				bool bEnabled = false;
				EnabledProp->GetValue(bEnabled);
				return bEnabled ? EVisibility::Visible : EVisibility::Collapsed;
			})
		];
	
	HeaderBox->AddSlot()
		.VAlign(EVerticalAlignment::VAlign_Top)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SProperty, EnabledProp)
				.ShouldDisplayName(false)
			]
			+ SHorizontalBox::Slot().Padding(4.0f, 0.0f)
			[
				StructPropertyHandle->CreatePropertyNameWidget(DisplayNameOverride, DisplayToolTipOverride, bDisplayResetToDefault)
			]
		];
			
	PropertiesBox->AddSlot()
		[
			SNew(SProperty, TitleProp)
			.ShouldDisplayName(true)
			.DisplayName(LOCTEXT("AxisInstTitle", "Title"))
		];
	PropertiesBox->AddSlot()
		[
			SNew(SProperty, MarkersProp)
			.ShouldDisplayName(true)
			.DisplayName(LOCTEXT("AxisInstMarkers", "Markers"))
		];
	PropertiesBox->AddSlot()
		[
			SNew(SProperty, LabelsProp)
			.ShouldDisplayName(true)
			.DisplayName(LOCTEXT("AxisInstLabels", "Labels"))
		];
}

void FCartesianAxisInstanceConfigCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	
}


TSharedRef< IPropertyTypeCustomization > FCartesianAxisConfigCustomization::MakeInstance()
{
	return MakeShareable(new FCartesianAxisConfigCustomization);
}

void FCartesianAxisConfigCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	const bool bDisplayResetToDefault = false;
	const FText DisplayNameOverride = FText::GetEmpty();
	const FText DisplayToolTipOverride = FText::GetEmpty();

	auto ResetBtn = SNew(SResetToDefaultMenu);
	ResetBtn->AddProperty(StructPropertyHandle);

	HeaderRow
		.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget(DisplayNameOverride, DisplayToolTipOverride, bDisplayResetToDefault)
		]
		.ValueContent()
		[
			SNew(SVerticalBox).Visibility(EVisibility::Collapsed)
		]
		;
}

void FCartesianAxisConfigCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	uint32 NumChildren = 0;
	StructPropertyHandle->GetNumChildren(NumChildren);
	for (uint32 Idx = 0; Idx < NumChildren; ++Idx)
	{
		auto Handle = StructPropertyHandle->GetChildHandle(Idx).ToSharedRef();

		if (Handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED(FCartesianAxisConfig, FloatingAxis))
		{
			if (StructPropertyHandle->HasMetaData(TEXT("DisableFloating")))
			{
				continue;
			}
		}

		auto& Child = StructBuilder.AddProperty(Handle);
	
		if (Handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED(FCartesianAxisConfig, LeftBottomAxis))
		{
			if (StructPropertyHandle->HasMetaData(TEXT("ConfigAxisType")))
			{
				auto Type = StructPropertyHandle->GetMetaData(TEXT("ConfigAxisType"));
				if (Type == TEXT("X"))
				{
					Child.DisplayName(LOCTEXT("BottomDisplayName", "Bottom Axis"));
				}
				else if (Type == TEXT("Y"))
				{
					Child.DisplayName(LOCTEXT("LeftDisplayName", "Left Axis"));
				}
			}
		}

		if (Handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED(FCartesianAxisConfig, RightTopAxis))
		{
			if (StructPropertyHandle->HasMetaData(TEXT("ConfigAxisType")))
			{
				auto Type = StructPropertyHandle->GetMetaData(TEXT("ConfigAxisType"));
				if (Type == TEXT("X"))
				{
					Child.DisplayName(LOCTEXT("TopDisplayName", "Top Axis"));
				}
				else if (Type == TEXT("Y"))
				{
					Child.DisplayName(LOCTEXT("RightDisplayName", "Right Axis"));
				}
			}
		}
	}
}


TSharedRef< IPropertyTypeCustomization > FCartesianRangeBoundCustomization::MakeInstance()
{
	return MakeShareable(new FCartesianRangeBoundCustomization);
}

void FCartesianRangeBoundCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	const bool bDisplayResetToDefault = false;
	const FText DisplayNameOverride = FText::GetEmpty();
	const FText DisplayToolTipOverride = FText::GetEmpty();

	auto BoundTypeProp = StructPropertyHandle->GetChildHandle(TEXT("Type"));
	auto FixedBoundProp = StructPropertyHandle->GetChildHandle(TEXT("FixedBoundValue"));

	HeaderRow
		.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget(DisplayNameOverride, DisplayToolTipOverride, bDisplayResetToDefault)
		]
		.ValueContent()
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SProperty, BoundTypeProp)
				.ShouldDisplayName(false)
			]
			+ SHorizontalBox::Slot().Padding(4.0f, 0.0f)
			.AutoWidth()
			[
				SNew(SProperty, FixedBoundProp)
				.ShouldDisplayName(false)
				.Visibility_Lambda([BoundTypeProp]
				{
					uint8 uBoundType;
					if (BoundTypeProp->GetValue(uBoundType) == FPropertyAccess::Success)
					{
						if (static_cast<ECartesianRangeBoundType>(uBoundType) == ECartesianRangeBoundType::FixedValue)
						{
							return EVisibility::Visible;
						}
					}
					return EVisibility::Collapsed;
				})
			]
		]
	;
}

void FCartesianRangeBoundCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	
}


#undef LOCTEXT_NAMESPACE


