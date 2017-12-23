#pragma once

#include <string>
#include <vector>

namespace ProjectBase
{

class Blob
{
public:
	Blob() = default;

	Blob(const std::string& name, size_t size = 0U)
		: origName{ name }
		, m_size{ size }
	{
	}

	Blob(const char name[], size_t size = 0U)
		: origName{ name }
		, m_size{ size }
	{
	}

	std::string Name() const
	{
		return origName;
	}

	size_t Size() const
	{
		return m_size;
	}

protected:
	size_t m_size;
	std::string origName;
};

}
