#pragma once

#include <string>
#include <vector>

namespace ProjectBase
{

class File
{
public:
	File() = default;

	File(const std::string& name)
		: origName{ name }
	{
	}

	File(const char name[])
		: origName{ name }
	{
	}

	std::string Name() const {
		return origName;
	}

private:
	std::string origName;
	std::vector<unsigned char> content;
};

}
