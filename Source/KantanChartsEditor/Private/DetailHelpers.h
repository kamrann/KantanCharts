// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "Widgets/SWidget.h"
#include "Templates/SharedPointer.h"
#include "Templates/SubclassOf.h"


class IPropertyHandle;
class UInterface;


namespace DetailHelpers {

	TSharedRef< SWidget > MakeInterfaceAssetPicker(
		TSharedRef< IPropertyHandle > Property,
		TSubclassOf< UInterface > InterfaceClass
		);

}


