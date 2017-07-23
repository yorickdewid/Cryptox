#pragma once

#include "ObjectStore.h"

namespace ProjectBase
{

class DiagramStore : public ObjectStore<File>
{
public:
	DiagramStore()
		: ObjectStore{ FactoryObjectType::ObjectTypeDiagramStore }
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
