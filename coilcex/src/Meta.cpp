// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Meta.h"

#include <boost/lexical_cast.hpp>

CryExe::Meta::ImageVersionCompound CryExe::Meta::ImageVersion(const Executable& exec)
{
	switch (exec.GetInternalImageVersion()) {
	case CryExe::InternalImageVersion::IMAGE_STRUCT_FORMAT_03:
	{
		const short version = exec.ImageVersion();
		return { static_cast<short>((version >> 8) & 0xff), static_cast<short>(version & 0xff) };
	}
	default:
		break;
	}

	return std::make_pair<short, short>(0, 0);
}

std::string CryExe::Meta::ProgramVersion(const Executable& exec)
{
	((void)exec);
	/*switch (exec.GetInternalProgramVersion()) {
	default:
		break;
	}*/

	return "";
}
