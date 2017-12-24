#pragma once

#include <string>
#include <fstream>

class SourceUnit
{
	std::string m_name;
	size_t m_fileSize;
	bool m_isInternalFile;

public:
	// The default constructor taks a filename as input and tries
	// to locate the file on disk
	SourceUnit(const std::string& sourceName, bool isInternalFile)
		: m_name{ sourceName }
		, m_isInternalFile{ isInternalFile }
	{
		OpenFile(m_name);
	}

	// Mock the source unit and do not try to open filename on disk,
	// this allows any overrides from implementing another source unit
	// object without filesystem requirements
	SourceUnit(const std::string& sourceName)
		: m_name{ sourceName }
		, m_isInternalFile{ false }
	{
	}

	// Allow source to by copied
	SourceUnit(const SourceUnit& other)
		: m_name{ other.m_name }
	{
		OpenFile(m_name);
	}

	// Allow source to by moved
	SourceUnit(SourceUnit&& other)
		: m_name{ other.m_name }
		, m_fileSize{ other.m_fileSize }
		, m_sizeLeft{ other.m_sizeLeft }
		, m_offset{ other.m_offset }
	{
		other.Close();

		OpenFile(m_name);
	}

	~SourceUnit()
	{
		Close();
	}

	virtual inline void Close()
	{
		// Release file resource on deconstruction
		if (m_sourceFile.is_open()) {
			m_sourceFile.close();
		}
	}

	// Return file size in bytes
	virtual size_t Size() const
	{
		return m_fileSize;
	}

	// Read contents from file
	virtual const std::string Read(size_t size)
	{
		std::string contentChunk;

		// No more contents to read
		if (m_sizeLeft == 0) {
			return "";
		}

		// Read less than hinted
		if (m_sizeLeft <= size) {
			size = m_sizeLeft;
		}

		contentChunk.resize(size);

		m_sourceFile.clear();
		m_sourceFile.seekg(m_offset, std::ios::beg);
		m_sourceFile.read(const_cast<char*>(contentChunk.data()), size);
		m_offset += contentChunk.size();
		m_sizeLeft -= contentChunk.size();

		return contentChunk;
	}

	virtual const std::string Name() const
	{
		return m_name;
	}

private:
	inline void OpenFile(const std::string filename)
	{
		m_sourceFile.open(filename, std::ios::in | std::ios::binary);
		m_fileSize = FindFileSize();
		m_sizeLeft = m_fileSize;
	}

	size_t FindFileSize()
	{
		m_sourceFile.seekg(0, std::ios::end);
		size_t fileSize = static_cast<size_t>(m_sourceFile.tellg());
		m_sourceFile.seekg(0, std::ios::beg);
		return fileSize;
	}

private:
	std::fstream m_sourceFile;
	size_t m_sizeLeft = 0;
	size_t m_offset = 0;
};
