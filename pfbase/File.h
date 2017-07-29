#pragma once

#include <string>
#include <vector>
#include <iostream>

#include <boost/lexical_cast.hpp>

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

	File(const std::string& name, const contentVector& content)
		: origName{ name }
		, m_size{ content.size() }
		, m_content{ content }
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

	contentVector Data() const
	{
		return m_content;
	}

	friend std::ostream& operator<<(std::ostream& out, const File& file)
	{
		return out << file.m_size << ':' << file.origName << ':' << file.m_content;
	}

	friend std::istream& operator>>(std::istream& in, File& file)
	{
		std::string token;
		std::getline(in, token, ':');
		file.m_size = boost::lexical_cast<size_t>(token);
		std::getline(in, token, ':');
		file.origName = token;
		std::getline(in, token, ':');
		file.m_content = token;

		return in;
	}

	// Stream in the data. The stream wil be appended to the internal content structure
	// and the object returns itself to allow chaining.
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
