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
// exposed via the meta interface and is readonly. All of the methods
// are static since there is no cohesion between the individual
// operations. The only reason for this structure is the access
// on the executable object.
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

	// Image version
	static ImageVersionCompound ImageVersion(const Executable&);

	// Test if the this image version is the lastest
	static bool IsLatestImageVersion(const Executable&);

	// Type of executable format
	static ExecType ImageType(const Executable&);

	// Offset to program structure since start of image
	static long long ImageProgramOffset(const Executable&);

	// Optional image flags
	static std::string ImageFlags(const Executable&);

	// Total size of image structure
	static long long ImageStructureSize(const Executable&);

	// Timestamp of image creation
	static ProgramTimestampClock ProgramTimestamp(const Executable&);

	// Subsystem target
	static short ProgramSubsystemTarget(const Executable&);

	// Subsystem version
	static short ProgramSubsystemVersion(const Executable&);

	// Total size of all code segments
	static long long ProgramCodeSize(const Executable&);

	// Allocated size for stack
	static long long ProgramStackSize(const Executable&);

	// Number of sections in the image
	static int ProgramSectionCount(const Executable&);

	// Number of directories in the image
	static int ProgramDirectoryCount(const Executable&);

	// Offset to first section
	static long long ProgramSectionOffset(const Executable&);

	// Offset to first directory
	static long long ProgramDirectoryOffset(const Executable&);

	// Total size of program structure
	static long long ProgramStructureSize(const Executable&);

	struct SectionIntric
	{
		// Section data size
		static size_t ImageDataSize(const Section&);

		// Section image offset
		static long long ImageOffset(const Section&);
	};
};

} // namespace CryExe
