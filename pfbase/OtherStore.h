#pragma once

#include "ObjectStore.h"
#include "File.h"

namespace ProjectBase
{

class OtherStore : public ObjectStore<File>
{
public:
	OtherStore()
		: ObjectStore{ Store::ObjectTypeOtherStore }
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
		out << "worst";
	}

};

}
