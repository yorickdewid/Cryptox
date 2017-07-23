#pragma once

#include "ObjectStore.h"

namespace ProjectBase
{

class OtherStore : public ObjectStore<File>
{
public:
	OtherStore()
		: OtherStore{ FactoryObjectType::ObjectTypeOtherStore }
	{
	}

	void AddFile(const char file[])
	{
		AddNode(file);
	}

	void AddFile(File& file)
	{
		AddNode(file);
	}
};

}
