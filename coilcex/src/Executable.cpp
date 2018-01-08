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

CryExe::Executable::Executable(const std::string& path, FileMode fm, ExecType type)
	: Image{ path }
{
	this->Open(fm);

	switch (type) {
	case CryExe::ExecType::TYPE_EXECUTABLE:
		m_execType = static_cast<int>(Structure::ExecutableType::CET_EXECUTABLE);
		break;
	case CryExe::ExecType::TYPE_LIBRARY_DYNAMIC:
		m_execType = static_cast<int>(Structure::ExecutableType::CET_DYNAMIC);
		break;
	}
}

CryExe::Executable::~Executable()
{
	this->Close();

	if (m_interalImageStructure) {
		delete m_interalImageStructure;
		m_interalImageStructure = nullptr;
	}
}

void CryExe::Executable::SetOption(Option options)
{
	PULL_INTSTRCT(imageFile);

	unsigned short flags = static_cast<unsigned short>(imageFile->imageHeader.flagsOptional);

	if (options & Option::OPT_READONLY) {
		flags |= static_cast<unsigned short>(CryExe::Structure::ImageFlags::CCH_READ_ONLY);
	}
	if (options & Option::OPT_BINREP) {
		flags |= static_cast<unsigned short>(CryExe::Structure::ImageFlags::CCH_BIN_REPRODUCE);
	}

	imageFile->imageHeader.flagsOptional = static_cast<CryExe::Structure::ImageFlags>(flags);
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

		// Get all headers from sections and directories into memory
		// in order to speedup initial search
		ConveySectionsFromDisk();
		ConveyDirectoriesFromDisk();
	}
}

void CryExe::Executable::ConveySectionsFromDisk()
{
	//
}

void CryExe::Executable::ConveyDirectoriesFromDisk()
{
	//
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

std::pair<int, bool> CryExe::Executable::ResolveSectionType(Section::SectionType inType)
{
#define ALLOW_ONCE true
#define ALLOW_MULTI false

	switch (inType) {
	case CryExe::Section::NATIVE:
		return { Structure::CexSection::SectionIdentifier::DOT_TEXT, ALLOW_MULTI };
	case CryExe::Section::RESOURCE:
		return { Structure::CexSection::SectionIdentifier::DOT_RSRC, ALLOW_ONCE };
	case CryExe::Section::DATA:
		return { Structure::CexSection::SectionIdentifier::DOT_DATA, ALLOW_MULTI };
	case CryExe::Section::DEBUG:
		return { Structure::CexSection::SectionIdentifier::DOT_DEBUG, ALLOW_MULTI };
	case CryExe::Section::SOURCE:
		return { Structure::CexSection::SectionIdentifier::DOT_SRC, ALLOW_ONCE };
	case CryExe::Section::NOTE:
		return { Structure::CexSection::SectionIdentifier::DOT_NOTE, ALLOW_MULTI };
	default:
		break;
	}

	return { Structure::CexSection::SectionIdentifier::_INVAL, 0 };

#undef ALLOW_ONCE
#undef ALLOW_MULTI
}

void CryExe::Executable::AddDirectory()
{
	//IsAllowedOnce();
}

void CryExe::Executable::AddSection(Section *section)
{
	assert(section);

	const auto& datablock = section->Data();
	if (IsSealed()) {
		throw std::runtime_error{ "sealed images cannot amend contents" };
	}

	auto typePair = ResolveSectionType(section->Type());
	assert(typePair.first != Structure::CexSection::SectionIdentifier::_INVAL);

	// Test if section was used before
	if (typePair.second) {
		if (m_allocSections.test(typePair.first)) {
			throw std::runtime_error{ "section allowed only once per image" };
		}
		m_allocSections.set(typePair.first);
	}

	Structure::CexSection rawSection;
	MEMZERO(rawSection, sizeof(Structure::CexSection));
	rawSection.identifier = static_cast<Structure::CexSection::SectionIdentifier>(typePair.first);
	rawSection.flags = Structure::SectionCharacteristic::SC_NONE;
	rawSection.offsetToSection = UNASSIGNED;
	rawSection.sizeOfArray = datablock.size();
	SETSTRUCTSZ(rawSection, Structure::CexSection);

	// Save current image offset to stack
	m_offsetStackSection.push_back(static_cast<size_t>(m_file.Offset()));

	// Commit to disk
	m_file.Write(rawSection);
	m_file.Write((*datablock.data()), datablock.size());
	//section->Clear();
}

std::vector<CryExe::Section> CryExe::Executable::Sections()
{
	return { CryExe::Section{ CryExe::Section::SectionType::RESOURCE } };
}

void CryExe::Executable::GetSectionDataFromImage(Section& section)
{
	assert(section.InternalDataOffset() != ILLEGAL_OFFSET);
	if (!section.Empty()) { return; }
	if (section.InternalDataSize() == 0) { return; }

	// Move file to the offset image position
	m_file.Position(section.InternalDataOffset());

	ByteArray bArray;
	bArray.resize(section.InternalDataSize());
	MEMZERO(bArray, section.InternalDataSize());
	m_file.Read(bArray, section.InternalDataSize());

	// Move data array into section
	section.Emplace(std::move(bArray));
}

short CryExe::Executable::ImageVersion() const
{
	PULL_INTSTRCT(imageFile);

	std::uint16_t version;
	version = (imageFile->imageHeader.versionMajor << 8) & 0xff00;
	version |= imageFile->imageHeader.versionMinor;

	return version;
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

	{
		// If any of the preliminary checks fail, abort right away
		if (!memcmp(tmpImageHeader.identArray, Structure::identity, sizeof(Structure::identity))) {
			m_interalImageVersion = InternalImageVersion::IMAGE_STRUCT_FORMAT_03;
		}
		if (tmpImageHeader.versionMajor != IMAGE_VERSION_MAJOR || tmpImageHeader.versionMinor != IMAGE_VERSION_MINOR) {
			throw std::runtime_error{ EXCEPT_INVAL_CEX };
		}
		if (tmpImageHeader.structSize != sizeof(Structure::CexImageHeader)) {
			throw std::runtime_error{ EXCEPT_INVAL_CEX };
		}

		if (m_interalImageVersion == InternalImageVersion::IMAGE_STRUCT_FORMAT_INVAL) {
			throw std::runtime_error{ EXCEPT_INVAL_CEX };
		}
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

	// Consume end of header marker
	std::uint16_t marker;
	m_file.Read(marker);
	if (marker != 0xfefe) {
		throw std::runtime_error{ EXCEPT_INVAL_CEX };
	}
}

void CryExe::Executable::CreateNewImage()
{
	Structure::CexFileFormat imageFile;
	MEMZERO(imageFile, sizeof(Structure::CexFileFormat));

	assert(m_file.IsOpen());

	// Default image header
	imageFile.imageHeader.versionMajor = IMAGE_VERSION_MAJOR;
	imageFile.imageHeader.versionMinor = IMAGE_VERSION_MINOR;
	imageFile.imageHeader.executableType = static_cast<Structure::ExecutableType>(m_execType);
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

	// End of header marker
	std::uint16_t marker = 0xfefe;
	m_file.Write(marker);

	// Write file header opaque to memory
	m_interalImageStructure = new Structure::CexFileFormat;
	MEMASSIGN(m_interalImageStructure, sizeof(Structure::CexFileFormat), &imageFile, sizeof(Structure::CexFileFormat));
}

void CryExe::Executable::CalculateInternalOffsets()
{
	PULL_INTSTRCT(imageFile);

	// Program header follows directly behind image header
	imageFile->imageHeader.offsetToProgram = sizeof(Structure::CexImageHeader);

	// Fetch offset from offset deques
	if (!m_offsetStackSection.empty()) {
		imageFile->programHeader.numberOfSections = static_cast<std::uint16_t>(m_offsetStackSection.size());
		imageFile->programHeader.offsetToSectionTable = m_offsetStackSection.front();
		m_offsetStackSection.pop_front();
	}
	if (!m_offsetStackDirectory.empty()) {
		imageFile->programHeader.numberOfDirectories = static_cast<std::uint16_t>(m_offsetStackDirectory.size());
		imageFile->programHeader.offsetToDirectoryTable = m_offsetStackDirectory.front();
		m_offsetStackDirectory.pop_front();
	}
}

void CryExe::Executable::CalculateImageSize()
{
	PULL_INTSTRCT(imageFile);

	// Move to end of image
	m_file.Forward();

	// Size of the code is everything of value that was added to the image.
	// The actual size of the file on disk is most certainly different from this
	// number as the image is padded at the end.
	imageFile->programHeader.sizeOfCode = static_cast<std::uint32_t>(m_file.Offset());
}

void CryExe::Executable::CalculateSectionOffsets()
{
	PULL_INTSTRCT(imageFile);

	size_t sectionOffset = imageFile->programHeader.offsetToSectionTable;
	while (sectionOffset) {
		sectionOffset += offsetof(Structure::CexSection, offsetToSection);
		decltype(Structure::CexSection::offsetToSection) memberOffset = 0;

		// Last section does not point to anything
		if (!m_offsetStackSection.empty()) {
			memberOffset = static_cast<decltype(Structure::CexSection::offsetToSection)>(m_offsetStackSection.front());
			m_offsetStackSection.pop_front();
		}

		m_file.Position(sectionOffset);
		m_file.Rewrite(memberOffset);
		sectionOffset = memberOffset;
	}
}

void CryExe::Executable::CalculateDirectoryOffsets()
{
	PULL_INTSTRCT(imageFile);

	//TODO
}

void CryExe::Executable::AlignBounds()
{
#define ALIGNMENT (1 << 5)
	std::fpos_t offset = m_file.Offset();
	assert(offset > 0);

	// Align image on 1 << 5 bytes boundary
	if (offset % ALIGNMENT > 0) {
		size_t left = offset % ALIGNMENT;
		uint8_t *align = new uint8_t[left];
		MEMZERO((*align), left);
		assert(left < ALIGNMENT);

		// Move to end of image and write padding
		m_file.Forward();
		m_file.Write((*align), left);
		delete[] align;
	}
#undef ALIGNMENT
}

const CryExe::Executable& CryExe::Executable::Seal(CryExe::Executable& exec)
{
	Structure::CexFileFormat& imageFile = static_cast<Structure::CexFileFormat&>(*reinterpret_cast<Structure::CexFileFormat*>(exec.m_interalImageStructure));

	assert(exec.m_file.IsOpen());

	// Set identity string
	MEMASSIGN(imageFile.imageHeader.identArray, sizeof(imageFile.imageHeader.identArray), Structure::identity, sizeof(Structure::identity));

	// Calculate structure offsets and update the image and program headers
	exec.CalculateInternalOffsets();
	exec.CalculateImageSize();
	exec.CalculateSectionOffsets();
	exec.CalculateDirectoryOffsets();

	// Make sure the image is bounds aligned
	exec.AlignBounds();

	// Set file descriptor at beginning of file and write the image file to disk
	exec.m_file.Rewind();
	exec.m_file.Rewrite(imageFile);

	return exec;
}

#undef EXCEPT_INVAL_CEX
