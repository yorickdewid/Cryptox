#pragma once

#include "ObjectStore.h"

namespace ProjectBase
{

class MaterialStore : public ObjectStore
{
public:
	MaterialStore()
		: ObjectStore{ FactoryObjectType::ObjectTypeMaterialStore }
	{
	}

};

}
