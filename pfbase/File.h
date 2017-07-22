#pragma once

#include <string>

namespace ProjectBase
{

class File
{
public:
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
};

}
