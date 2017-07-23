#pragma once

#include <string>
#include <vector>

namespace ProjectBase
{

typedef std::basic_string<char> contentVector;

class File
{
public:
	File() = default;

	File(const std::string& name, size_t size = 0U)
		: origName{ name }
		, m_size{ size }
	{
	}

	File(const char name[], size_t size = 0U)
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

	File& operator<<(const std::string& content)
	{
		m_content += content;
		m_size = m_content.size();
		return *this;
	}

private:
	size_t m_size;
	std::string origName;
	contentVector m_content;
};

}
