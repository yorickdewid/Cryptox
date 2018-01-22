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
#include <chrono>

namespace CryExe
{

// Meta is a helper class to access the internal information about
// an image, program or sections. This information can only be 
// exposed via the meta interface and is readonly.
struct COILCEXAPI Meta final
{
	// Image version pair respectively major and minor part
	using ImageVersionCompound = std::pair<short, short>;

	// Timepoint of program timestamp
	using ProgramTimestampClock = std::chrono::time_point<std::chrono::system_clock>;

	// Image identity for current implementation
	static std::string StructureIdentity();

	// Image magic value for current implementation
	static int StructureMagic();

	// Return the image version
	static ImageVersionCompound ImageVersion(const Executable&);

	// Test if the this image version is the lastest
	static bool IsLatestImageVersion(const Executable&);

	// Return the program version as string
	static std::string ProgramVersion(const Executable&);

	static ExecType ImageType(const Executable&);

	static long long ImageProgramOffset(const Executable&);

	static std::string ImageFlags(const Executable&);

	static long long ImageStructureSize(const Executable&);

	static ProgramTimestampClock ProgramTimestamp(const Executable&);

	static short ProgramSubsystemTarget(const Executable&);

	static short ProgramSubsystemVersion(const Executable&);

	static long long ProgramCodeSize(const Executable&);

	static long long ProgramStackSize(const Executable&);

	static int ProgramSectionCount(const Executable&);

	static int ProgramDirectoryCount(const Executable&);

	static long long ProgramSectionOffset(const Executable&);

	static long long ProgramDirectoryOffset(const Executable&);

	static long long ProgramStructureSize(const Executable&);
};

} // namespace CryExe
