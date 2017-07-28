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

	void Print(std::ostream& out) const override
	{
		out << "ham";
	}

	void Parse(const std::string content)
	{
		//
	}

};

}
