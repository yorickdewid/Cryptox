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

std::chrono::milliseconds ChronoTimestamp()
{
	using namespace std::chrono;
	
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch());
}

CryExe::Executable::Executable(const std::string& path, FileMode fm)
	: Image{ path, fm }
{
	// Open if required
	OpenWithMode(fm);

	//If image was opened with previous action, validate CEX structure
	if (IsOpen()) {
		ValidateImageFormat();
	}
	else {
		CreateNewImage();
	}
}

void CryExe::Executable::ValidateImageFormat()
{
	//
}

void CryExe::Executable::CreateNewImage()
{
	FILE *fpImage = nullptr;

	fopen_s(&fpImage, "kaas.cex", "w+b");
	assert(fpImage);

	Structure::CexFileFormat imageFile;
	std::memset(static_cast<void*>(&imageFile), '\0', sizeof(Structure::CexFileFormat));
	memcpy_s(imageFile.imageHeader.identArray, sizeof(imageFile.imageHeader.identArray), Structure::identity, sizeof(Structure::identity));
	
	// Default image header
	imageFile.imageHeader.versionMajor = IMAGE_VERSION_MAJOR;
	imageFile.imageHeader.versionMajor = IMAGE_VERSION_MINOR;
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
	std::fwrite(static_cast<const void*>(&imageFile), sizeof(Structure::CexFileFormat), 1, fpImage);

#if 0
	Structure::CexNoteSection node;
	std::memset(static_cast<void*>(&node), '\0', sizeof(Structure::CexNoteSection));
	node.identifier = Structure::CexSection::SectionIdentifier::DOT_NOTE;
	SETSTRUCTSZ(node, Structure::CexNoteSection);

	// Commit to disk
	std::fwrite(static_cast<const void*>(&node), sizeof(Structure::CexNoteSection), 1, fpImage);
#endif

	std::fclose(fpImage);
}
