// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "FileReader.h"

namespace fs = boost::filesystem;

// Check if file is a valid source file
bool IsValidSourceFile(const fs::path& file)
{
	if (!fs::exists(file)) { return false; }
	if (!fs::is_regular_file(file)) { return false; }
	if (fs::is_empty(file)) { return false; }

	return true;
}

// Loop over the directory search paths and check if the unit can
// be found in one of the directories. The search paths are iterated
// in order so that the algorithm is deterministic.
bool FindValidSourceFile(std::deque<fs::path> sourcePathList,  fs::path& unitPath)
{
	for (const auto searchPath : sourcePathList) {
		fs::path unitFullPath{ searchPath };
		unitFullPath /= unitPath;
		if (IsValidSourceFile(unitFullPath)) {
			unitPath = unitFullPath;
			return true;
		}
	}

	return false;
}

void FileReader::AppendFileToList(const std::string& filename)
{
	fs::path unitPath{ filename };
	if (unitPath.has_parent_path()) {
		if (!IsValidSourceFile(unitPath)) {
			throw std::system_error{ std::make_error_code(std::errc::no_such_file_or_directory) };
		}
		if (!unitPath.is_absolute()) {
			unitPath = fs::canonical(unitPath);
		}
	}
	else {
		if (!FindValidSourceFile(m_sourcePaths, unitPath)) {
			throw std::system_error{ std::make_error_code(std::errc::no_such_file_or_directory) };
		}
	}

	// Iff this is the first source file, then add the directory of the source file to the search
	// path. If the first source references another source file relatively, then it is a good
	// assumption the referenced source can be found in the same working directory.
	if (IsFirstUnit()) {
		if (unitPath.has_parent_path() && unitPath.is_absolute()) {
			m_sourcePaths.push_front(unitPath.parent_path());
		}
	}

	m_unitList.push(std::make_unique<SourceUnit>(SourceUnit{ unitPath.string() }));
}
