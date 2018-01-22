// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Exportable.h"
#include "OSAdapter.h"

#include <string>

namespace CryExe
{

enum class COILCEXAPI FileMode
{
	FM_OPEN,		// Open the image file if it exits, otherwise throw
	FM_NEW,			// Open new file, discard any old files with the same name
};

using FileModeRaw = const char *;

class COILCEXAPI Image
{
	// Image file name on disk
	const std::string m_filename;
	
	// Extract the base file name from the path
	static std::string GetBasenameFromPath(const std::string& path);

protected:
	OSAdapter m_file;

public:
	Image(const std::string& filename)
		: m_filename{ filename }
	{
	}

	// Return filename
	inline std::string Name() const { return GetBasenameFromPath(m_filename); }

	// Return file size
	size_t Size() const;

	// Check if the file is open
	bool IsOpen() const { return m_file.IsOpen(); }

	// Open file, the internals will take care of already open files
	void Open(FileMode mode)
	{
		if (IsOpen()) { return; }

		m_file.Open(m_filename, OpenWithMode(mode));
	};

	// Close file without explicit flush
	void Close() { m_file.Close(); }

	// Flush buffer contents to disk
	void Flush() { m_file.Flush(); }

protected:
	static FileModeRaw OpenWithMode(FileMode fm);
};

} // namespace CryExecutable

