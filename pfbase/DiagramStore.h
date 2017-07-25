#pragma once

#include "ObjectStore.h"
#include "File.h"

namespace ProjectBase
{

class DiagramStore : public ObjectStore<File>
{
public:
	DiagramStore()
		: ObjectStore{ Store::ObjectTypeDiagramStore }
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

	void Print(std::ostream& out) const override
	{
		out << "kaas";
	}

};

}
