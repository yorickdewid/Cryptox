#pragma once

#include "Reader.h"

#include <string>

class FileReader : public Reader
{
public:
	FileReader(const std::string& filename)
	{
		/*if (!boost::filesystem::exists(fname))
		{
		throw std::system_error{ std::make_error_code(std::errc::no_such_file_or_directory) };
		}*/
	}

private:
	/*void FileToProg()
	{
		std::ifstream src{ fname };

		std::string str;
		src.seekg(0, std::ios::end);
		str.reserve(static_cast<size_t>(src.tellg()));
		src.seekg(0, std::ios::beg);

		str.assign((std::istreambuf_iterator<char>(src)),
			std::istreambuf_iterator<char>());

		m_content = str;
	}*/

};

