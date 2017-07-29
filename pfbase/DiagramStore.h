#pragma once

#include "ObjectStore.h"
#include "File.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <sstream>

constexpr unsigned char marker[4] = { '\x1','\x7','\x2','\x4' };

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

	File& GetFile(const char file[])
	{
		return GetNode(file);
	}

	// Write internal store contents and files to output stream
	void Print(std::ostream& out) const override
	{
		out << nodeList.size();
		for (auto& file : nodeList) {
			out << file << marker;
		}
	}

	void Parse(const std::string content)
	{
		size_t nodeCount = boost::lexical_cast<size_t>(content.substr(0, 1));

		std::vector<std::string> results;
		boost::split(results, content.substr(1), boost::is_any_of(marker));

		for (auto& fcontent : results) {
			if (fcontent.empty()) {
				continue;
			}

			std::istringstream in(fcontent);

			File file;
			in >> file;

			AddFile(file);
		}
	}

};

}
