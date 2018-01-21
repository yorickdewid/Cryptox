// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <Cry/Indep.h>

#include "Executable.h"
#include "Convert.h"
#include "CexHeader.h"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <chrono>

#define SETSTRUCTSZ(s,f) \
	s.structSize = sizeof(f);

#define EXCEPT_INVAL_CEX "invalid CEX format"

const std::chrono::milliseconds ChronoTimestamp()
{
	using namespace std::chrono;

	return duration_cast<milliseconds>(system_clock::now().time_since_epoch());
}

constexpr static const size_t StackSectionPosition = 0;
constexpr static const size_t StackDirectoryPosition = 1;

#define ALLOW_ONCE true
#define ALLOW_MULTI false

using SectionNativePair = std::pair<CryExe::Structure::CexSection::SectionIdentifier, bool>;

SectionNativePair ResolveSectionTypeToIdentifier(CryExe::Section::SectionType inType)
{
	switch (inType) {
	case CryExe::Section::SectionType::NATIVE:
		return { CryExe::Structure::CexSection::SectionIdentifier::DOT_TEXT, ALLOW_MULTI };
	case CryExe::Section::SectionType::RESOURCE:
		return { CryExe::Structure::CexSection::SectionIdentifier::DOT_RSRC, ALLOW_ONCE };
	case CryExe::Section::SectionType::DATA:
		return { CryExe::Structure::CexSection::SectionIdentifier::DOT_DATA, ALLOW_MULTI };
	case CryExe::Section::SectionType::DEBUG:
		return { CryExe::Structure::CexSection::SectionIdentifier::DOT_DEBUG, ALLOW_MULTI };
	case CryExe::Section::SectionType::SOURCE:
		return { CryExe::Structure::CexSection::SectionIdentifier::DOT_SRC, ALLOW_ONCE };
	case CryExe::Section::SectionType::NOTE:
		return { CryExe::Structure::CexSection::SectionIdentifier::DOT_NOTE, ALLOW_MULTI };
	default:
		break;
	}

	throw std::exception{};
}

CryExe::Section::SectionType ResolveIdentifierToSectionType(CryExe::Structure::CexSection::SectionIdentifier inId)
{
	switch (inId) {
	case CryExe::Structure::CexSection::DOT_TEXT:
		return CryExe::Section::SectionType::NATIVE;
	case CryExe::Structure::CexSection::DOT_RSRC:
		return CryExe::Section::SectionType::RESOURCE;
	case CryExe::Structure::CexSection::DOT_DATA:
		return CryExe::Section::SectionType::DATA;
	case CryExe::Structure::CexSection::DOT_DEBUG:
		return CryExe::Section::SectionType::DEBUG;
	case CryExe::Structure::CexSection::DOT_SRC:
		return CryExe::Section::SectionType::SOURCE;
	case CryExe::Structure::CexSection::DOT_NOTE:
		return CryExe::Section::SectionType::NOTE;
	default:
		break;
	}

	throw std::runtime_error{ EXCEPT_INVAL_CEX };
}

#undef ALLOW_ONCE
#undef ALLOW_MULTI

CryExe::Executable::Executable(const std::string& path, FileMode fm, ExecType type)
	: Image{ path }
{
	// Register type
	switch (type) {
	case CryExe::ExecType::TYPE_EXECUTABLE:
		m_execType = static_cast<int>(Structure::ExecutableType::CET_EXECUTABLE);
		break;
	case CryExe::ExecType::TYPE_LIBRARY_DYNAMIC:
		m_execType = static_cast<int>(Structure::ExecutableType::CET_DYNAMIC);
		break;
	}

	this->Open(fm);
}

CryExe::Executable::Executable(CryExe::Executable& exe, FileMode fm)
	: Image{ exe.Name() }
{
	if (fm == FileMode::FM_OPEN) {
		m_interalImageVersion = exe.m_interalImageVersion;
		m_execType = exe.m_execType;

		// If the image on disk was still open, close it
		// to prevent locking issues on certain platforms.
		if (exe.m_file.IsOpen()) {
			exe.Close();
		}
	}

	this->Open(fm);
}

CryExe::Executable::~Executable()
{
	this->Close();
}

bool CryExe::Executable::IsExecutable() const
{
	assert(m_interalImageStructure);
	return m_interalImageStructure->imageHeader.executableType == Structure::ExecutableType::CET_EXECUTABLE;
}

bool CryExe::Executable::IsDynamicLibrary() const
{
	assert(m_interalImageStructure);
	return m_interalImageStructure->imageHeader.executableType == Structure::ExecutableType::CET_DYNAMIC;
}

CryExe::SectionList::iterator CryExe::Executable::FindSection(CryExe::Section::SectionType type)
{
	return std::find_if(m_foundSectionList.begin(), m_foundSectionList.end(), [&type](const CryExe::Section& section)
	{
		return section.Type() == type;
	});
}

void CryExe::Executable::SetOption(Option options)
{
	assert(m_interalImageStructure);

	unsigned short flags = static_cast<unsigned short>(m_interalImageStructure->imageHeader.flagsOptional);

	if (options & Option::OPT_READONLY) {
		flags |= static_cast<unsigned short>(CryExe::Structure::ImageFlags::CCH_READ_ONLY);
	}
	if (options & Option::OPT_BINREP) {
		flags |= static_cast<unsigned short>(CryExe::Structure::ImageFlags::CCH_BIN_REPRODUCE);
	}

	m_interalImageStructure->imageHeader.flagsOptional = static_cast<CryExe::Structure::ImageFlags>(flags);
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
	assert(m_interalImageStructure);

	// Skip if there are no sections in this image, otherwise
	// reserve the items on the section list.
	if (m_interalImageStructure->programHeader.numberOfSections == 0) { return; }
	m_foundSectionList.reserve(m_interalImageStructure->programHeader.numberOfSections);

	assert(m_interalImageStructure->programHeader.offsetToSectionTable);
	OSFilePosition offset = m_interalImageStructure->programHeader.offsetToSectionTable;

	Structure::CexSection rawSection;
	while (offset) {
		CRY_MEMZERO(rawSection, sizeof(Structure::CexSection));
		m_file.Position(offset);
		m_file.Read(rawSection);

		Section section{ ResolveIdentifierToSectionType(rawSection.identifier) };
		section.m_dataPosition.internalImageDataOffset = offset + sizeof(Structure::CexSection);
		section.m_dataPosition.internalImageDataSize = rawSection.sizeOfArray;

		m_foundSectionList.push_back(std::move(section));
		offset = rawSection.offsetToSection;
	}
}

void CryExe::Executable::ConveyDirectoriesFromDisk()
{
	assert(m_interalImageStructure);

	// TODO
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

void CryExe::Executable::AddDirectory(Directory* directory)
{
	assert(directory);

	if (IsSealed()) {
		throw std::runtime_error{ "sealed images cannot amend contents" };
	}

	// Prepare directory structure to be written to disk
	Structure::CexDirectory rawDirectory;
	CRY_MEMZERO(rawDirectory, sizeof(Structure::CexDirectory));
	SETSTRUCTSZ(rawDirectory, Structure::CexDirectory);

	if (!m_offsetStack) {
		m_offsetStack = std::make_unique<std::array<std::deque<OSFilePosition>, 2>>();
	}
	m_offsetStack->at(StackDirectoryPosition).push_back(m_file.Offset());
	
	// Commit to disk
	m_file.Write(rawDirectory);
}

void CryExe::Executable::AddSection(Section *section)
{
	assert(section);

	if (IsSealed()) {
		throw std::runtime_error{ "sealed images cannot amend contents" };
	}

	SectionNativePair typePair = ResolveSectionTypeToIdentifier(section->Type());
	assert(typePair.first != Structure::CexSection::SectionIdentifier::_INVAL);

	// Test if section was used before
	if (typePair.second) {
		if (m_allocSections.test(typePair.first)) {
			throw std::runtime_error{ "section allowed only once per image" };
		}
		m_allocSections.set(typePair.first);
	}

	Convert::operations_type ops = 0;
	if (section->storageOptionFlags.test(STORAGE_OPTION_ENCRYPTION)) {
		ops |= Convert::Operations::CO_ENCRYPT;
	}
	if (section->storageOptionFlags.test(STORAGE_OPTION_COMPRESSION)) {
		ops |= Convert::Operations::CO_COMPRESS;
	}

	// Allow derived objects to swap data into section
	section->DataSwap(Section::DataSwapDirection::DATA_SWAP_OUT);

	// Perform data processing operations in-place
	Convert{ section->data, ops }.ToImage();

	// Prepare section structure to be written to disk
	Structure::CexSection rawSection;
	CRY_MEMZERO(rawSection, sizeof(Structure::CexSection));
	rawSection.identifier = static_cast<Structure::CexSection::SectionIdentifier>(typePair.first);
	rawSection.flags = Structure::SectionCharacteristic::SC_NONE;
	rawSection.offsetToSection = UNASSIGNED;
	rawSection.sizeOfArray = section->Size();
	SETSTRUCTSZ(rawSection, Structure::CexSection);

	// Save current image offset to stack
	if (!m_offsetStack) {
		m_offsetStack = std::make_unique<std::array<std::deque<OSFilePosition>, 2>>();
	}
	m_offsetStack->at(StackSectionPosition).push_back(m_file.Offset());

	// Commit to disk
	m_file.Write(rawSection);
	m_file.Write((*section->Data().data()), section->Size());
	section->Clear();
}

void CryExe::Executable::GetSection(Section *section)
{
	assert(section);
	if (Sections().empty()) { return; }

	auto it = FindSection(section->type);
	if (it == Sections().cend()) {
		throw std::exception{};
	}

	Section& foundSection = (*it);
	GetSectionDataFromImage(foundSection);

	// Copy data into section and clear data from section list
	(*section) = foundSection;
	foundSection.Clear();

	// Notify derived section data is ready
	section->DataSwap(Section::DataSwapDirection::DATA_SWAP_IN);
}

void CryExe::Executable::GetSectionDataFromImage(Section& section) //TODO: throw when not found
{
	assert(section.InternalDataOffset() != ILLEGAL_OFFSET);
	if (!section.Empty()) { return; }
	if (section.InternalDataSize() == 0) { return; }

	// Move file to the offset image position
	m_file.Position(section.InternalDataOffset());

	ByteArray bArray;
	bArray.resize(section.InternalDataSize());
	CRY_MEMZERO((*bArray.data()), section.InternalDataSize());
	m_file.Read((*bArray.data()), section.InternalDataSize());

	Convert::operations_type ops = 0;
	if (section.storageOptionFlags.test(STORAGE_OPTION_ENCRYPTION)) {
		ops |= Convert::Operations::CO_ENCRYPT;
	}
	if (section.storageOptionFlags.test(STORAGE_OPTION_COMPRESSION)) {
		ops |= Convert::Operations::CO_COMPRESS;
	}

	// Perform data processing operations in-place
	Convert{ bArray, ops }.FromImage();

	// Move data array into section
	section.Emplace(std::move(bArray));
}

short CryExe::Executable::ImageVersion() const
{
	assert(m_interalImageStructure);

	std::uint16_t version;
	version = (m_interalImageStructure->imageHeader.versionMajor << 8) & 0xff00;
	version |= m_interalImageStructure->imageHeader.versionMinor;

	return version;
}

bool CryExe::Executable::IsSealed() const
{
	assert(m_interalImageStructure);

	// If the image identifier was written the image is sealed and becomes readonly
	if (!memcmp(m_interalImageStructure->imageHeader.identArray, Structure::identity, sizeof(Structure::identity))) {
		return true;
	}

	return false;
}

void CryExe::Executable::ValidateImageFormat()
{
	Structure::CexFileFormat imageFile;
	CRY_MEMZERO(imageFile, sizeof(Structure::CexFileFormat));

	assert(m_file.IsOpen());

	// Read image header from disk. Since the image header should never
	// change we can verifiy if the structure contains a valid CEX format
	Structure::CexImageHeader tmpImageHeader;
	CRY_MEMZERO(tmpImageHeader, sizeof(Structure::CexImageHeader));
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
	m_interalImageStructure = std::make_unique<Structure::CexFileFormat>(imageFile);

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
	CRY_MEMZERO(imageFile, sizeof(Structure::CexFileFormat));

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
	m_interalImageStructure = std::make_unique<Structure::CexFileFormat>(imageFile);
}

void CryExe::Executable::CalculateInternalOffsets()
{
	assert(m_interalImageStructure);

	// Program header follows directly behind image header
	m_interalImageStructure->imageHeader.offsetToProgram = sizeof(Structure::CexImageHeader);

	// Fetch offset from offset deques
	if (m_offsetStack && !m_offsetStack->at(StackSectionPosition).empty()) {
		m_interalImageStructure->programHeader.numberOfSections = static_cast<std::uint16_t>(m_offsetStack->at(StackSectionPosition).size());
		m_interalImageStructure->programHeader.offsetToSectionTable = m_offsetStack->at(StackSectionPosition).front().Position<decltype(m_interalImageStructure->programHeader.offsetToSectionTable)>();
		m_offsetStack->at(StackSectionPosition).pop_front();
	}
	if (m_offsetStack && !m_offsetStack->at(StackDirectoryPosition).empty()) {
		m_interalImageStructure->programHeader.numberOfDirectories = static_cast<std::uint16_t>(m_offsetStack->at(StackDirectoryPosition).size());
		m_interalImageStructure->programHeader.offsetToDirectoryTable = m_offsetStack->at(StackDirectoryPosition).front().Position<decltype(m_interalImageStructure->programHeader.offsetToDirectoryTable)>();
		m_offsetStack->at(StackDirectoryPosition).pop_front();
	}
}

void CryExe::Executable::CalculateImageSize()
{
	assert(m_interalImageStructure);

	// Move to end of image
	m_file.Forward();

	// Size of the code is everything of value that was added to the image.
	// The actual size of the file on disk is most certainly different from this
	// number as the image is padded at the end.
	m_interalImageStructure->programHeader.sizeOfCode = static_cast<std::uint32_t>(m_file.Offset());
}

void CryExe::Executable::CalculateSectionOffsets()
{
	assert(m_interalImageStructure);

	size_t sectionOffset = m_interalImageStructure->programHeader.offsetToSectionTable;
	while (sectionOffset) {
		sectionOffset += offsetof(Structure::CexSection, offsetToSection);
		decltype(Structure::CexSection::offsetToSection) memberOffset = 0;

		// Last section does not point to anything
		if (m_offsetStack && !m_offsetStack->at(StackSectionPosition).empty()) {
			memberOffset = m_offsetStack->at(StackSectionPosition).front().Position<decltype(Structure::CexSection::offsetToSection)>();
			m_offsetStack->at(StackSectionPosition).pop_front();
		}

		m_file.Position(sectionOffset);
		m_file.Rewrite(memberOffset);
		sectionOffset = memberOffset;
	}
}

void CryExe::Executable::CalculateDirectoryOffsets()
{
	assert(m_interalImageStructure);

	//TODO
}

void CryExe::Executable::AlignBounds()
{
#define ALIGNMENT (1 << 5)
	auto offset = m_file.Offset().NativePosition();
	assert(offset > 0);

	// Align image on ALIGNMENT boundary. This ensures
	// all images are of the same size which can increase
	// the performance as blocks of data can be read at once.
	if (offset % ALIGNMENT > 0) {
		size_t left = offset % ALIGNMENT;
		uint8_t *align = new uint8_t[left];
		CRY_MEMZERO((*align), left);
		assert(left < ALIGNMENT);

		// Move to end of image and write padding as bytes
		// of zero. The padding byes must always be zero since
		// this will guarantee binary reproducibility.
		m_file.Forward();
		m_file.Write((*align), left);
		delete[] align;
	}
#undef ALIGNMENT
}

const CryExe::Executable& CryExe::Executable::Seal(CryExe::Executable& exec)
{
	assert(exec.m_file.IsOpen());

	// Set identity string
	CRY_MEMCPY(exec.m_interalImageStructure->imageHeader.identArray, sizeof(exec.m_interalImageStructure->imageHeader.identArray), Structure::identity, sizeof(Structure::identity));

	// Calculate structure offsets and update the image and program headers
	exec.CalculateInternalOffsets();
	exec.CalculateImageSize();
	exec.CalculateSectionOffsets();
	exec.CalculateDirectoryOffsets();

	// Make sure the image is bounds aligned
	exec.AlignBounds();

	// Set file descriptor at beginning of file and write the image file to disk
	exec.m_file.Rewind();
	exec.m_file.Rewrite(exec.m_interalImageStructure->imageHeader);
	exec.m_file.Rewrite(exec.m_interalImageStructure->programHeader);

	return exec;
}

#undef EXCEPT_INVAL_CEX
