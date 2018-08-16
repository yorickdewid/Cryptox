// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

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
	// to locate the file on disk.
	SourceUnit(const std::string& sourceName, bool isInternalFile);

	// Mock the source unit and do not try to open filename on disk,
	// this allows any overrides from implementing another source unit
	// object without filesystem requirements.
	SourceUnit(const std::string& sourceName);

	// Allow source to by copied.
	SourceUnit(const SourceUnit&);

	// Allow source to by moved.
	SourceUnit(SourceUnit&& other);

	// Close any open resources.
	virtual ~SourceUnit();

	// Close any open resources.
	virtual void Close();

	// Return file size in bytes.
	virtual size_t Size() const noexcept { return m_fileSize; }
	// Check if file is empty.
	virtual bool Empty() const noexcept { return m_fileSize == 0; }

	// Read contents from file.
	virtual const std::string Read(size_t);

	// Retrieve the name of the source unit.
	virtual const std::string Name() const noexcept { return m_name; }

private:
	void OpenFile(const std::string filename);

	size_t FindFileSize();

private:
	std::fstream m_sourceFile;
	size_t m_sizeLeft{ 0 };
	size_t m_offset{ 0 };
};
