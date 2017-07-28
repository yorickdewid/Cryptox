#pragma once

#include "ObjectStore.h"
#include "File.h"

#include <boost/lexical_cast.hpp>

#include <sstream>

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

	// Write internal store contents and files to output stream
	void Print(std::ostream& out) const override
	{
		out << nodeList.size();
		for (auto& file : nodeList) {
			out << file;
		}
	}

	void Parse(const std::string content)
	{
		size_t nodeCount = boost::lexical_cast<size_t>(content.substr(0, 1));

		std::istringstream in(content.substr(1));

		File x;
		in >> x;

		AddFile(x);
	}

};

}
