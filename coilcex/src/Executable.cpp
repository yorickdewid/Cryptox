// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Executable.h"
#include "CexHeader.h"

#include <cassert>
#include <cstdio>
#include <chrono>

#define SETSTRUCTSZ(s,f) \
	s.structSize = sizeof(f);

#define MEMZERO(b,s) \
	memset(static_cast<void*>(&b), '\0', s);

#ifdef _WIN32
# define MEMASSIGN(d,u,s) \
	memcpy_s(d, sizeof(d), u, s);
#else
# define MEMASSIGN(d,u,s) \
	memcpy(d, u, s);
#endif

std::chrono::milliseconds ChronoTimestamp()
{
	using namespace std::chrono;

	return duration_cast<milliseconds>(system_clock::now().time_since_epoch());
}

CryExe::Executable::Executable(const std::string& path, FileMode fm)
	: Image{ path }
{
	this->Open(fm);
}

void CryExe::Executable::Open(FileMode mode)
{
	Image::Open(mode);

	if (mode == FileMode::FM_NEW) {
		CreateNewImage();
	}
	else {
		ValidateImageFormat();
	}
}

void CryExe::Executable::ValidateImageFormat()
{
	assert(m_file.IsOpen());

	Structure::CexFileFormat imageFile;
	MEMZERO(imageFile, sizeof(Structure::CexFileFormat));

	// Read image header from disk. Since the image header should never
	// change we can verifiy if the structure contains a valid CEX format
	Structure::CexImageHeader tmpImageHeader;
	MEMZERO(tmpImageHeader, sizeof(Structure::CexImageHeader));
	m_file.Read(tmpImageHeader);

#define EXCEPT_INVAL_CEX "invalid CEX format"
	
	// If any of the preliminary checks fail, abort right away
	if (memcmp(tmpImageHeader.identArray, Structure::identity, sizeof(Structure::identity))) {
		throw std::runtime_error{ EXCEPT_INVAL_CEX };
	}
	if (tmpImageHeader.versionMajor != IMAGE_VERSION_MAJOR || tmpImageHeader.versionMinor != IMAGE_VERSION_MINOR) {
		throw std::runtime_error{ EXCEPT_INVAL_CEX };
	}
	if (tmpImageHeader.structSize != sizeof(Structure::CexImageHeader)) {
		throw std::runtime_error{ EXCEPT_INVAL_CEX };
	}

	// Looks like this file contains a CEX image, read the whole thing at once
	m_file.Read(imageFile);
	if (imageFile.programHeader.magic != PROGRAM_MAGIC) {
		throw std::runtime_error{ EXCEPT_INVAL_CEX };
	}

#undef EXCEPT_INVAL_CEX
}

void CryExe::Executable::CreateNewImage()
{
	Structure::CexFileFormat imageFile;
	MEMZERO(imageFile, sizeof(Structure::CexFileFormat));

	assert(m_file.IsOpen());

	MEMASSIGN(imageFile.imageHeader.identArray, Structure::identity, sizeof(Structure::identity));

	// Default image header
	imageFile.imageHeader.versionMajor = IMAGE_VERSION_MAJOR;
	imageFile.imageHeader.versionMinor = IMAGE_VERSION_MINOR;
	imageFile.imageHeader.executableType = Structure::ExecutableType::CET_NONE;
	imageFile.imageHeader.flagsOptional = Structure::ImageFlags::CCH_NONE;
	imageFile.imageHeader.offsetToProgram = 0;
	SETSTRUCTSZ(imageFile.imageHeader, Structure::CexImageHeader);

	// Default program header
	imageFile.programHeader.magic = PROGRAM_MAGIC;
	imageFile.programHeader.timestampDate = ChronoTimestamp().count();
	imageFile.programHeader.subsystemVersion = PROGRAM_SUBSYS_VERSION;
	imageFile.programHeader.subsystemTarget = PROGRAM_SUBSYSTEM;
	imageFile.programHeader.sizeOfCode = 0;
	imageFile.programHeader.sizeOfStack = PROGRAM_DEFAULT_STACK_SZ;
	imageFile.programHeader.numberOfSections = 0;
	imageFile.programHeader.numberOfDirectories = 0;
	imageFile.programHeader.offsetToSectionTable = 0;
	imageFile.programHeader.offsetToDirectoryTable = 0;
	imageFile.programHeader.characteristics = Structure::ProgramCharacteristic::PC_NONE;
	SETSTRUCTSZ(imageFile.programHeader, Structure::CexProgramHeader);

	// Commit to disk
	m_file.Write(imageFile);

#if 0
	Structure::CexNoteSection node;
	std::memset(static_cast<void*>(&node), '\0', sizeof(Structure::CexNoteSection));
	node.identifier = Structure::CexSection::SectionIdentifier::DOT_NOTE;
	SETSTRUCTSZ(node, Structure::CexNoteSection);

	// Commit to disk
	std::fwrite(static_cast<const void*>(&node), sizeof(Structure::CexNoteSection), 1, fpImage);
#endif
}
