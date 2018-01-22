// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "HeaderDump.h"

#include <sstream>
#include <iostream>
#include <iomanip>
#include <ctime>

using namespace CryExe;

namespace Detail
{

constexpr unsigned long long operator "" _KB(unsigned long long)
{
	return 1024;
}

std::string ImageVersion(const CryExe::Executable& exec)
{
	const Meta::ImageVersionCompound& imageVersion = Meta::ImageVersion(exec);

	std::stringstream ss;
	ss << imageVersion.first << "." << imageVersion.second;

	if (Meta::IsLatestImageVersion(exec)) {
		ss << " (current)";
	}

	return ss.str();
}

std::string ImageIdentifier()
{
	return Meta::StructureIdentity();
}

std::string ImageType(const CryExe::Executable& exec)
{
	try {
		ExecType type = Meta::ImageType(exec);

		std::stringstream ss;

		switch (type) {
		case ExecType::TYPE_EXECUTABLE:
			ss << "TYPE_EXECUTABLE";
			ss << " (Executable file)";
			break;
		case ExecType::TYPE_LIBRARY_DYNAMIC:
			ss << "TYPE_LIBRARY_DYNAMIC";
			ss << " (Dynamic library)";
			break;
		default:
			throw std::bad_cast{};
		}

		return ss.str();
	}
	catch (std::bad_cast&) {
		return "[UNKNOWN]";
	}
}

std::string ImageProgramOffset(const CryExe::Executable& exec)
{
	return std::to_string(Meta::ImageProgramOffset(exec)) + " (bytes into file)";
}

std::string ImageFlags(const CryExe::Executable& exec)
{
	return Meta::ImageFlags(exec);
}

std::string ImageStructureSize(const CryExe::Executable& exec)
{
	return std::to_string(Meta::ImageStructureSize(exec)) + " (bytes)";
}

std::string ProgramMagic()
{
	std::stringstream ss;

	ss << std::hex << Meta::StructureMagic();

	return ss.str();
}

std::string ProgramTimestamp(const CryExe::Executable& exec)
{
	Meta::ProgramTimestampClock ts = Meta::ProgramTimestamp(exec);
	
	std::time_t t = std::chrono::system_clock::to_time_t(ts);
	std::string timestring{ std::ctime(&t) };
	timestring.pop_back();
	
	return timestring;
}

std::string ProgramSubsystemTarget(const CryExe::Executable& exec)
{
	return "Advanced Micro Devices x86_64";
}

std::string ProgramSubsystemVersion(const CryExe::Executable& exec)
{
	return "0x1";
}

std::string ProgramCodeSize(const CryExe::Executable& exec)
{
	return std::to_string(Meta::ProgramCodeSize(exec)) + " (bytes)";
}

std::string ProgramStackSize(const CryExe::Executable& exec)
{
	auto sizInKB = Meta::ProgramStackSize(exec) / 1_KB;
	return std::to_string(sizInKB) + " (kilobytes)";
}

std::string ProgramSectionCount(const CryExe::Executable& exec)
{
	return std::to_string(Meta::ProgramSectionCount(exec));
}

std::string ProgramDirectoryCount(const CryExe::Executable& exec)
{
	return std::to_string(Meta::ProgramDirectoryCount(exec));
}

std::string ProgramSectionOffset(const CryExe::Executable& exec)
{
	return std::to_string(Meta::ProgramSectionOffset(exec)) + " (bytes into file)";
}

std::string ProgramDirectoryOffset(const CryExe::Executable& exec)
{
	return std::to_string(Meta::ProgramDirectoryOffset(exec)) + " (bytes into file)";
}

std::string ProgramStructureSize(const CryExe::Executable& exec)
{
	return std::to_string(Meta::ProgramStructureSize(exec)) + " (bytes)";
}

} // namespace Detail

void HeaderDump::ParseImageHeader(const CryExe::Executable& exec)
{
	std::cout << "Image Header:" << '\n'
		<< "  Version:                   " << Detail::ImageVersion(exec) << '\n'
		<< "  Identification:            " << Detail::ImageIdentifier() << '\n'
		<< "  Type:                      " << Detail::ImageType(exec) << '\n'
		<< "  Start of program headers:  " << Detail::ImageProgramOffset(exec) << '\n'
		<< "  Flags:                     " << Detail::ImageFlags(exec) << '\n'
		<< "  Size of this header:       " << Detail::ImageStructureSize(exec) << '\n'
		<< std::flush;
}

void HeaderDump::ParseProgramHeader(const CryExe::Executable& exec)
{
	const std::string programVersion = Meta::ProgramVersion(exec);

	std::cout << "Program Header:" << '\n'
		<< "  Magic:                     " << Detail::ProgramMagic() << '\n'
		<< "  Timestamp:                 " << Detail::ProgramTimestamp(exec) << '\n'
		<< "  Subsystem Target:          " << Detail::ProgramSubsystemTarget(exec) << '\n'
		<< "  Subsystem Version:         " << Detail::ProgramSubsystemVersion(exec) << '\n'
		<< "  Size of Code Segment:      " << Detail::ProgramCodeSize(exec) << '\n'
		<< "  Size of Stack:             " << Detail::ProgramStackSize(exec) << '\n'
		<< "  Number of Sections:        " << Detail::ProgramSectionCount(exec) << '\n'
		<< "  Number of Directories:     " << Detail::ProgramDirectoryCount(exec) << '\n'
		<< "  Start of Sections:         " << Detail::ProgramSectionOffset(exec) << '\n'
		<< "  Start of Directories:      " << Detail::ProgramDirectoryOffset(exec) << '\n'
		<< "  Size of this header:       " << Detail::ProgramStructureSize(exec) << '\n'
		<< std::flush;
}
