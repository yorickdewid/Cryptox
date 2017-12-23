// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <string>

namespace CryExe
{

enum class FileMode
{
	FM_OPEN,		// Open the image file if it exits, otherwise throw
	FM_OPEN_RO,		// Open in readonly mode and only if the file exists
	FM_OPEN_COMMIT,	// Open if the file exists and otherwise create a new file
};

class Image
{
	const std::string& m_filename;

	static std::string GetBasenameFromPath(const std::string& path);

public:
	Image(const std::string& filename, FileMode fm = FileMode::FM_OPEN)
		: m_filename{ GetBasenameFromPath(filename) }
	{
		OpenWithMode(fm);
	}

	inline bool IsOpen() const { return false; } //TODO
	
	void Open();
	void Close();

private:
	void OpenWithMode(FileMode fm);
};

} // namespace CryExecutable
