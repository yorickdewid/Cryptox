// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Image.h"

#include <boost/filesystem.hpp>

std::string CryExe::Image::GetBasenameFromPath(const std::string& path)
{
	return boost::filesystem::path{ path }.filename().string();
}

size_t CryExe::Image::Size() const
{
	return boost::filesystem::path{ m_filename }.size();
}

CryExe::FileModeRaw CryExe::Image::OpenWithMode(CryExe::FileMode fm)
{
	switch (fm) {
	case CryExe::FileMode::FM_OPEN:
		return "r+b";
	case CryExe::FileMode::FM_OPEN_RO:
		return "rb";
	case CryExe::FileMode::FM_NEW:
		return "wb";
	default:
		break;
	}

	return "rb";
}
