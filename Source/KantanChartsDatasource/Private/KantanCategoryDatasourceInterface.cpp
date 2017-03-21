// Copyright (C) 2015-2017 Cameron Angus. All Rights Reserved.

#include "KantanCategoryDatasourceInterface.h"


int32 IKantanCategoryDatasourceInterface::GetNumCategories_Implementation() const
{
	return 0;
}

FName IKantanCategoryDatasourceInterface::GetCategoryId_Implementation(int32 CatIdx) const
{
	return NAME_None;
}

FText IKantanCategoryDatasourceInterface::GetCategoryName_Implementation(int32 CatIdx) const
{
	return{};
}

float IKantanCategoryDatasourceInterface::GetCategoryValue_Implementation(int32 CatIdx) const
{
	return 0.0f;
}


