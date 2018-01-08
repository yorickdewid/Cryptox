// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Exportable.h"
#include "Image.h"
#include "Section.h"

#include <string>
#include <deque>
#include <bitset>

namespace CryExe
{

class COILCEXAPI InvalidCexFormat
{
};

enum class COILCEXAPI InternalImageVersion
{
	IMAGE_STRUCT_FORMAT_INVAL = 0,
	IMAGE_STRUCT_FORMAT_03 = 3,
};

enum class COILCEXAPI ExecType
{
	TYPE_EXECUTABLE,
	TYPE_LIBRARY_DYNAMIC,
};

class COILCEXAPI Executable : public Image
{
	void *m_interalImageStructure = nullptr;
	InternalImageVersion m_interalImageVersion = InternalImageVersion::IMAGE_STRUCT_FORMAT_INVAL;
	std::deque<size_t> m_offsetStackSection;
	std::deque<size_t> m_offsetStackDirectory;
	std::bitset<UINT16_MAX> m_allocSections = 0;
	int m_execType;

public:
	enum class COILCEXAPI Option
	{
		OPT_BINREP = 1 << 1,
		OPT_READONLY = 1 << 2,
	};

	friend Option operator|(Option opt1, Option opt2)
	{
		return static_cast<Option>(static_cast<int>(opt1) | static_cast<int>(opt2));
	}

	friend int operator&(Option opt1, Option opt2)
	{
		return static_cast<int>(opt1) & static_cast<int>(opt2);
	}

public:
	Executable(const std::string& path, FileMode fm = FileMode::FM_OPEN, ExecType type = ExecType::TYPE_EXECUTABLE);
	~Executable();

	// Check if the image is sealed and thus readonly
	bool IsSealed() const;

	// Open executable with file mode
	void Open(FileMode);

	// Close image handler
	void Close();

	// Add new directory to CEX image
	void AddDirectory();

	// Add new section to CEX image
	void AddSection(Section *);
	
	std::vector<CryExe::Section> Sections();

	// Fill structure
	void GetSectionDataFromImage(Section&);

	short ImageVersion() const;

	//TODO: friend?
	inline InternalImageVersion GetInternalImageVersion() const { return m_interalImageVersion; }
	inline int GetInternalProgramVersion() const { return 0; }

	void SetOption(Option);

	// Seal the executable in order to generate a valid CEX image. The sealing
	// process guantees a valid CEX is generated and the object cannot be 
	// altered when commited to disk.
	static const Executable& Seal(Executable&);

private:
	static std::pair<int, bool> CryExe::Executable::ResolveSectionType(Section::SectionType);

private:
	void ValidateImageFormat();
	void CreateNewImage();
	void AlignBounds();
	void CalculateInternalOffsets();
	void CalculateImageSize();
	void CalculateSectionOffsets();
	void CalculateDirectoryOffsets();
};

class COILCEXAPI DynamicLibrary : public Executable
{
public:
	template<typename... _TyArgs>
	DynamicLibrary(_TyArgs&&... args)
		: Executable{ std::forward<_TyArgs>(args)..., ExecType::TYPE_LIBRARY_DYNAMIC }
	{
	}
};

} // namespace CryExe
