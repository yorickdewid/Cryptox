// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Exportable.h"
#include "Executable.h"

#include <string>

namespace CryExe
{

struct COILCEXAPI Meta final
{
	// Image version pair respectively major and minor part
	using ImageVersionCompound = std::pair<short, short>;

	// Return the image version
	static ImageVersionCompound ImageVersion(const Executable&);

	// Test if the this image version is the lastest
	static bool IsLatestImageVersion(const Executable&);

	// Return the program version as string
	static std::string ProgramVersion(const Executable&);

	static std::string StructureIdentity();

	static ExecType ImageType(const Executable&);

	static long long ImageProgramOffset(const Executable&);

	static std::string ImageFlags(const Executable&);

	static long long ImageStructureSize(const Executable&);

	static long long ProgramStackSize(const Executable&);

	static int ProgramSectionCount(const Executable&);

	static int ProgramDirectoryCount(const Executable&);
};

} // namespace CryExe
