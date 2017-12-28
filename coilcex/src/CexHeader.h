// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <stdint.h>

#pragma pack(push, 1)

#define PROGRAM_MAGIC		0xdead0007
#define PROGRAM_SUBSYSTEM	0x4

#define PROGRAM_SUBSYS_VERSION	0x40

#define PROGRAM_SUBSYS_TARGET_INDEP	0x00
#define PROGRAM_SUBSYS_TARGET_WIN32	0x01
#define PROGRAM_SUBSYS_TARGET_OSX	0x02
#define PROGRAM_SUBSYS_TARGET_LINUX	0x04
#define PROGRAM_SUBSYS_TARGET_OBSD	0x08
#define PROGRAM_SUBSYS_TARGET_FBSD	0x10
#define PROGRAM_SUBSYS_TARGET_SOLAR	0x20

constexpr const unsigned char identity[8] = { 'C','R','Y','E','X','0','3','\0' };

enum class CexExecutableType : uint8_t
{
	CET_NONE,
	CET_RELOCATABLE,
	CET_EXECUTABLE,
	CET_DYNAMIC,
	CET_STATIC,
};

enum class CexImageFlags : uint16_t
{
	CCH_NONE = 1 << 0,
	CCH_READ_ONLY = 1 << 1,
	CCH_BIN_REPRODUCE = 1 << 2,
};

// Size: 20 bytes aligned
struct CexImageHeader
{
	// The first and constant header must always be identifiable by its
	// characteristic name and version. The identifier is human readable
	// to make visual identification possible when parsed wrongly.
	unsigned char identArray[8];

	// Version information detailing which version of the CEX
	// header is used. This is required for feature version of the
	// CEX header. The manjor and minor version parts are separated
	// into two field for easy and fast version determination.
	uint8_t versionMajor;
	uint8_t versionMinor;

	// The executable type what type of data this object contains.
	// Based on the executable type specific headers or object sections
	// may or may not be available.
	CexExecutableType executableType;

	CexImageFlags flagsOptional;

	uint32_t offsetToProgram;
	
	// Reserved fields are used to increase the change of natural page alignment
	// without compiler extensions required. Reserved fields may be used later
	// to extend the structure and therefore allow future options to be backwards
	// compatible with known structure sizes.
	uint8_t reserved;
	uint8_t reserved;

	// Size of the current structure must be set to find altered versions in
	// the future. The called must set the size via the sizeof() expression.
	// If the structure ever changes the parser is able to skip over the structure
	// and continue on to the image.
	uint8_t structSize;
};

enum class ProgramCharacteristic : uint16_t
{
	PC_NONE = 1 << 0,
	PC_RUN_NATIVE = 1 << 1,
	PC_RUN_IN_TIME = 1 << 2,
	PC_RUN_INTERACTIVE = 1 << 3,
	PC_INSTR_EXPANSION = 1 << 4,
	PC_ALLOW_AUTO_FALLBACK = 1 << 5,
};

// 40
struct CexProgramHeader
{
	// Magic value must always have the same value. The value is used to exclude any
	// parser mistakes or data corruption. When the magic values does not match the 
	// constant, any parsing operation must be aborted.
	uint32_t magic;

	// Timestamp of image creation in 64 bits precision. This timestamp can be used to
	// determine if there have been alterations made to the code. If the binary 
	// reproducibility flag is set the image header the timestamp must be zero.
	uint64_t timestampDate;

	uint8_t subsystemVersion;
	uint8_t subsystemTarget;

	uint32_t sizeOfCode;
	uint32_t sizeOfStack;

	uint16_t numberOfSections;
	uint16_t numberOfDirectories;

	uint32_t offsetToSectionTable;
	uint32_t offsetToDirectoryTable;

	ProgramCharacteristic characteristics;

	// Reserved fields are used to increase the change of natural page alignment
	// without compiler extensions required. Reserved fields may be used later
	// to extend the structure and therefore allow future options to be backwards
	// compatible with known structure sizes.
	uint8_t reserved;
	uint8_t reserved;
	uint8_t reserved;

	// Size of the current structure must be set to find altered versions in
	// the future. The called must set the size via the sizeof() expression.
	// If the structure ever changes the parser is able to skip over the structure
	// and continue on to the image.
	uint8_t structSize;
};

struct CexDirectory {};
struct CexSymbolTable : public CexDirectory {};
struct CexProtectionTable : public CexDirectory {};

enum class SectionCharacteristic : uint16_t
{
	SC_NONE = 1 << 0,
	SC_COMPRESSED = 1 << 1,
	SC_PACKED = 1 << 2,
	SC_LARGE_PAYLOAD = 1 << 3,
};

struct CexSection
{
	enum SectionIdentifier : uint16_t
	{
		DOT_TEXT,	// .text
		DOT_RSRC,	// .rsrc
		DOT_DATA,	// .data
		DOT_RDATA,	// .rdata
		DOT_IATA,	// .idata
		DOT_EDATA,	// .edata
		DOT_DEBUG,	// .debug
		DOT_SRC,	// .src
		DOT_NOTE,	// .note
	} identifier;

	SectionCharacteristic flags;

	uint8_t alignment;

	// Reserved fields are used to increase the change of natural page alignment
	// without compiler extensions required. Reserved fields may be used later
	// to extend the structure and therefore allow future options to be backwards
	// compatible with known structure sizes.
	uint8_t reserved;

	// Size of the current structure must be set to find altered versions in
	// the future. The called must set the size via the sizeof() expression.
	// If the structure ever changes the parser is able to skip over the structure
	// and continue on to the image.
	uint8_t structSize;
};

struct CexTextSection : public CexSection {};
struct CexResourceSection : public CexSection {};
struct CexDataSection : public CexSection {};
struct CexReadonlyDataSection : public CexSection {};
struct CexImportDataSection : public CexSection {};
struct CexExportDataSection : public CexSection {};
struct CexDebugSection : public CexSection {};
struct CexSourceSection : public CexSection {};
struct CexNoteSection : public CexSection {};

struct CexFileFormat
{
	CexImageHeader imageHeader;
	CexProgramHeader programHeader;
};

static_assert(sizeof(CexImageHeader) == 20, "CexImageHeader must have constant number of bytes");
static_assert(sizeof(CexProgramHeader) == 40, "CexProgramHeader must have constant number of bytes");

#pragma pack(pop)
