#pragma once

#include "Reader.h"

#include <boost/filesystem.hpp>

class FileReader : public Reader
{
public:
	FileReader(const std::string& filename)
		: origFilename{ filename }
	{
		//XXX: Switch to C++17
		if (!boost::filesystem::exists(filename)) {
			throw std::system_error{ std::make_error_code(std::errc::no_such_file_or_directory) };
		}

		sourceFile.open(origFilename, std::ios::in | std::ios::binary);
	}

	~FileReader()
	{
		// Release file resource on deconstruction
		if (sourceFile.is_open()) {
			sourceFile.close();
		}
	}

	virtual std::string FetchNextChunk(size_t sizeHint)
	{
		std::string contentChunk;
		contentChunk.resize(sizeHint);

		sourceFile.clear();
		sourceFile.seekg(fpOffset, std::ios::beg);
		sourceFile.read(const_cast<char*>(contentChunk.data()), sizeHint);
		fpOffset += contentChunk.size();

		return contentChunk;
	}

	virtual std::string FetchMetaInfo()
	{
		return origFilename;
	}

private:
	void FileToProg()
	{
		//std::ifstream src{ origFilename };

		//std::string str;
		/*src.seekg(0, std::ios::end);
		str.reserve(static_cast<size_t>(src.tellg()));
		src.seekg(0, std::ios::beg);

		str.assign((std::istreambuf_iterator<char>(src)),
			std::istreambuf_iterator<char>());

		m_content = str;*/
	}

private:
	size_t fpOffset = 0;
	std::fstream sourceFile;
	std::string origFilename;
};

