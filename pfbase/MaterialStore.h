#pragma once

#include "ObjectStore.h"
#include "Blob.h"

namespace ProjectBase
{

class MaterialStore : public ObjectStore<Blob>
{
public:
	MaterialStore()
		: ObjectStore{ Store::ObjectTypeMaterialStore }
	{
	}

};

}
