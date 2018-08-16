// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "SourceUnit.h"

SourceUnit::SourceUnit(const std::string& sourceName, bool isInternalFile)
	: m_name{ sourceName }
	, m_isInternalFile{ isInternalFile }
{
	OpenFile(m_name);
}

SourceUnit::SourceUnit(const std::string& sourceName)
	: m_name{ sourceName }
	, m_isInternalFile{ false }
{
}

SourceUnit::SourceUnit(const SourceUnit& other)
	: m_name{ other.m_name }
{
	OpenFile(m_name);
}

SourceUnit::SourceUnit(SourceUnit&& other)
	: m_name{ other.m_name }
	, m_fileSize{ other.m_fileSize }
	, m_sizeLeft{ other.m_sizeLeft }
	, m_offset{ other.m_offset }
{
	other.Close();

	OpenFile(m_name);
}

SourceUnit::~SourceUnit()
{
	Close();
}

void SourceUnit::Close()
{
	// Release file resource on deconstruction.
	if (m_sourceFile.is_open()) {
		m_sourceFile.close();
	}
}

const std::string SourceUnit::Read(size_t size)
{
	std::string contentChunk;

	// No more contents to read.
	if (m_sizeLeft == 0) {
		return "";
	}

	// Read less than hinted.
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

void SourceUnit::OpenFile(const std::string filename)
{
	m_sourceFile.open(filename, std::ios::in | std::ios::binary);
	m_fileSize = FindFileSize();
	m_sizeLeft = m_fileSize;
}

size_t SourceUnit::FindFileSize()
{
	m_sourceFile.seekg(0, std::ios::end);
	size_t fileSize = static_cast<size_t>(m_sourceFile.tellg());
	m_sourceFile.seekg(0, std::ios::beg);
	return fileSize;
}
