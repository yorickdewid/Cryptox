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

#define PULL_INTSTRCT(n) \
	Structure::CexFileFormat *n = reinterpret_cast<Structure::CexFileFormat*>(m_interalImageStructure); \
	assert(n);

#ifdef _WIN32
# define MEMASSIGN(d,i,u,s) \
	memcpy_s(d, i, u, s);
#else
# define MEMASSIGN(d,i,u,s) \
	memcpy(d, u, s);
#endif

#define UNASSIGNED 0

#define EXCEPT_INVAL_CEX "invalid CEX format"

const std::chrono::milliseconds ChronoTimestamp()
{
	using namespace std::chrono;

	return duration_cast<milliseconds>(system_clock::now().time_since_epoch());
}

CryExe::Executable::Executable(const std::string& path, FileMode fm)
	: Image{ path }
{
	this->Open(fm);
}

CryExe::Executable::~Executable()
{
	this->Close();

	if (m_interalImageStructure) {
		delete m_interalImageStructure;
	}
}

void CryExe::Executable::Open(FileMode mode)
{
	// Let parent open the image
	Image::Open(mode);

	// Either create new image or open an existing one
	if (mode == FileMode::FM_NEW) {
		CreateNewImage();
	}
	else {
		ValidateImageFormat();
	}
}

void CryExe::Executable::Close()
{
	// If file is closed without sealing, do it now
	if (!IsSealed()) {
		Seal(*this);
	}

	// Let parent close the image
	Image::Close();
}

void CryExe::Executable::AddDirectory()
{
	//IsAllowedOnce();
}

void CryExe::Executable::AddSection(Section *section)
{
	assert(section);
	if (IsSealed()) {
		throw std::runtime_error{ "sealed images cannot amend contents" };
	}
	if (!section->IsAllowedOnce()) {
		throw std::runtime_error{ "section allowed only once per image" };
	}
}

bool CryExe::Executable::IsSealed() const
{
	PULL_INTSTRCT(imageFile);

	// If the image identifier was written the image is sealed and becomes readonly
	if (!memcmp(imageFile->imageHeader.identArray, Structure::identity, sizeof(Structure::identity))) {
		return true;
	}

	return false;
}

void CryExe::Executable::ValidateImageFormat()
{
	Structure::CexFileFormat imageFile;
	MEMZERO(imageFile, sizeof(Structure::CexFileFormat));

	assert(m_file.IsOpen());

	// Read image header from disk. Since the image header should never
	// change we can verifiy if the structure contains a valid CEX format
	Structure::CexImageHeader tmpImageHeader;
	MEMZERO(tmpImageHeader, sizeof(Structure::CexImageHeader));
	m_file.Read(tmpImageHeader);

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
	m_file.Rewind();
	m_file.Read(imageFile);
	if (imageFile.programHeader.magic != PROGRAM_MAGIC) {
		throw std::runtime_error{ EXCEPT_INVAL_CEX };
	}

	// Write file header opaque to memory
	m_interalImageStructure = new Structure::CexFileFormat;
	MEMASSIGN(m_interalImageStructure, sizeof(Structure::CexFileFormat), &imageFile, sizeof(Structure::CexFileFormat));
}

void CryExe::Executable::CreateNewImage()
{
	Structure::CexFileFormat imageFile;
	MEMZERO(imageFile, sizeof(Structure::CexFileFormat));

	assert(m_file.IsOpen());

	// Default image header
	imageFile.imageHeader.versionMajor = IMAGE_VERSION_MAJOR;
	imageFile.imageHeader.versionMinor = IMAGE_VERSION_MINOR;
	imageFile.imageHeader.executableType = Structure::ExecutableType::CET_NONE;
	imageFile.imageHeader.flagsOptional = Structure::ImageFlags::CCH_NONE;
	imageFile.imageHeader.offsetToProgram = UNASSIGNED;
	SETSTRUCTSZ(imageFile.imageHeader, Structure::CexImageHeader);

	// Default program header
	imageFile.programHeader.magic = PROGRAM_MAGIC;
	imageFile.programHeader.timestampDate = ChronoTimestamp().count();
	imageFile.programHeader.subsystemVersion = PROGRAM_SUBSYS_VERSION;
	imageFile.programHeader.subsystemTarget = PROGRAM_SUBSYSTEM;
	imageFile.programHeader.sizeOfCode = UNASSIGNED;
	imageFile.programHeader.sizeOfStack = PROGRAM_DEFAULT_STACK_SZ;
	imageFile.programHeader.numberOfSections = UNASSIGNED;
	imageFile.programHeader.numberOfDirectories = UNASSIGNED;
	imageFile.programHeader.offsetToSectionTable = UNASSIGNED;
	imageFile.programHeader.offsetToDirectoryTable = UNASSIGNED;
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

	// Write file header opaque to memory
	m_interalImageStructure = new Structure::CexFileFormat;
	MEMASSIGN(m_interalImageStructure, sizeof(Structure::CexFileFormat), &imageFile, sizeof(Structure::CexFileFormat));
}

void CryExe::Executable::CalculateInternalOffsets()
{
	PULL_INTSTRCT(imageFile);

	// Program header follows directly behind image header
	imageFile->imageHeader.offsetToProgram = sizeof(Structure::CexImageHeader);

	//TODO:
	imageFile->programHeader.offsetToSectionTable = 0;
	imageFile->programHeader.offsetToDirectoryTable = 0;
}

void CryExe::Executable::CalculateImageSize()
{
	PULL_INTSTRCT(imageFile);

	imageFile->programHeader.sizeOfCode = sizeof(Structure::CexFileFormat);
}

const CryExe::Executable& CryExe::Executable::Seal(CryExe::Executable& exec)
{
	Structure::CexFileFormat& imageFile = static_cast<Structure::CexFileFormat&>(*reinterpret_cast<Structure::CexFileFormat*>(exec.m_interalImageStructure));

	// Set identity string
	MEMASSIGN(imageFile.imageHeader.identArray, sizeof(imageFile.imageHeader.identArray), Structure::identity, sizeof(Structure::identity));

	// Calculate structure offsets and update the image and program headers
	exec.CalculateInternalOffsets();
	exec.CalculateImageSize();

	// Set file descriptor at beginning of file and write the image file to disk
	exec.m_file.Rewind();
	exec.m_file.Write(imageFile);

	return exec;
}

#undef EXCEPT_INVAL_CEX
