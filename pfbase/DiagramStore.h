#pragma once

#include "ObjectStore.h"
#include "File.h"

#include <boost/algorithm/string.hpp>

#include <sstream>

namespace ProjectBase
{

constexpr unsigned char markerDiagram[4] = { '\001','\007','\002','\004' };

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

	void DeleteFile(const char file[])
	{
		DeleteNode(file);
	}

	// Write internal store contents and files to output stream
	void Print(std::ostream& out) const override
	{
		for (auto& file : nodeList) {
			out << file << '\001' << '\007' << '\002' << '\004';
		}
	}

	void Parse(const std::string content)
	{
		std::vector<std::string> results;
		boost::split(results, content, boost::is_any_of(markerDiagram));

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
