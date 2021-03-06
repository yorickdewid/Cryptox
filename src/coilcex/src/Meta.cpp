// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Meta.h"
#include "CexHeader.h"

#include <boost/lexical_cast.hpp>

//#define INTERNAL_IMAGE(e) e.m_interalImageStructure

#define INTERNAL_IMAGE(e,s) e.m_interalImageStructure->imageHeader.s
#define INTERNAL_PROGRAM(e,s) e.m_interalImageStructure->programHeader.s

using namespace CryExe;

Meta::ImageVersionCompound Meta::ImageVersion(const Executable& exec)
{
	switch (exec.m_interalImageVersion) {
	case InternalImageVersion::IMAGE_STRUCT_FORMAT_03:
	{
		const short version = exec.ImageVersion();
		return { static_cast<short>((version >> 8) & 0xff), static_cast<short>(version & 0xff) };
	}
	default:
		break;
	}

	return std::make_pair<short, short>(0, 0);
}

bool Meta::IsLatestImageVersion(const Executable& exec)
{
	return exec.m_interalImageVersion == InternalImageVersion::IMAGE_STRUCT_FOMART_LAST;
}

std::string Meta::StructureIdentity()
{
	return { std::begin(Structure::identity), std::end(Structure::identity) };
}

ExecType Meta::ImageType(const Executable& exec)
{
	switch (static_cast<Structure::ExecutableType>(exec.m_execType)) {
	case Structure::ExecutableType::CET_EXECUTABLE:
		return ExecType::TYPE_EXECUTABLE;
	case Structure::ExecutableType::CET_DYNAMIC:
		return ExecType::TYPE_LIBRARY_DYNAMIC;
	case Structure::ExecutableType::CET_NONE:
	case Structure::ExecutableType::CET_RELOCATABLE:
	case Structure::ExecutableType::CET_STATIC:
	default:
		break;
	}
	
	throw std::bad_cast{};
}

long long Meta::ImageProgramOffset(const Executable& exec)
{
	return INTERNAL_IMAGE(exec, offsetToProgram);
}

std::string Meta::ImageFlags(const Executable& exec)
{
	using IntergerCast = unsigned short;
	std::string outputFlags;
	
	IntergerCast flags = static_cast<IntergerCast>(INTERNAL_IMAGE(exec, flagsOptional));
	if (flags & static_cast<IntergerCast>(Structure::ImageFlags::CCH_READ_ONLY)) {
		if (outputFlags.size()) { outputFlags.push_back(','); }
		outputFlags.append("RO");
	}
	if (flags & static_cast<IntergerCast>(Structure::ImageFlags::CCH_BIN_REPRODUCE)) {
		if (outputFlags.size()) { outputFlags.push_back(','); }
		outputFlags.append("BINREP");
	}

	if (outputFlags.empty()) { outputFlags.push_back('-'); }

	return outputFlags;
}

long long Meta::ImageStructureSize(const Executable& exec)
{
	return INTERNAL_IMAGE(exec, structSize);
}

int Meta::StructureMagic()
{
	return PROGRAM_MAGIC;
}

Meta::ProgramTimestampClock Meta::ProgramTimestamp(const Executable& exec)
{
	using namespace std::chrono;

	return ProgramTimestampClock{ milliseconds{ INTERNAL_PROGRAM(exec, timestampDate) } };
}

short Meta::ProgramSubsystemTarget(const Executable& exec)
{
	return INTERNAL_PROGRAM(exec, subsystemTarget);
}

short Meta::ProgramSubsystemVersion(const Executable& exec)
{
	return INTERNAL_PROGRAM(exec, subsystemVersion);
}

long long Meta::ProgramCodeSize(const Executable& exec)
{
	return INTERNAL_PROGRAM(exec, sizeOfCode);
}

long long Meta::ProgramStackSize(const Executable& exec)
{
	return INTERNAL_PROGRAM(exec, sizeOfStack);
}

int Meta::ProgramSectionCount(const Executable& exec)
{
	return INTERNAL_PROGRAM(exec, numberOfSections);
}

int Meta::ProgramDirectoryCount(const Executable& exec)
{
	return INTERNAL_PROGRAM(exec, numberOfDirectories);
}

long long Meta::ProgramSectionOffset(const Executable& exec)
{
	return INTERNAL_PROGRAM(exec, offsetToSectionTable);
}

long long Meta::ProgramDirectoryOffset(const Executable& exec)
{
	return INTERNAL_PROGRAM(exec, offsetToDirectoryTable);
}

long long Meta::ProgramStructureSize(const Executable& exec)
{
	return INTERNAL_PROGRAM(exec, structSize);
}

size_t Meta::SectionIntric::ImageDataSize(const Section& section)
{
	return section.m_dataPosition.internalImageDataSize;
}

long long Meta::SectionIntric::ImageOffset(const Section& section)
{
	return section.m_dataPosition.internalImageDataOffset.NativePosition();
}
