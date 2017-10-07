#pragma once

#include <string>
#include <fstream>

class SourceUnit
{
	std::string m_name;
	size_t m_fileSize;

public:
	SourceUnit(const std::string& sourceName)
		: m_name{ sourceName }
	{
		OpenFile(m_name);
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
		if (other.m_sourceFile.is_open()) {
			other.m_sourceFile.close();
		}

		OpenFile(m_name);
	}

	~SourceUnit()
	{
		// Release file resource on deconstruction
		if (m_sourceFile.is_open()) {
			m_sourceFile.close();
		}
	}

	// Return file size in bytes
	size_t Size() const
	{
		return m_fileSize;
	}

	// Read contents from file
	const std::string Read(size_t size)
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

	const std::string Name() const
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
