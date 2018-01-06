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

class COILCEXAPI Executable : public Image
{
	void *m_interalImageStructure = nullptr;
	InternalImageVersion m_interalImageVersion = InternalImageVersion::IMAGE_STRUCT_FORMAT_INVAL;

public:
	Executable(const std::string& path, FileMode fm = FileMode::FM_OPEN);
	~Executable();

	// Check if the image is sealed and thus readonly
	bool IsSealed() const;

	// Open executable with file mode
	void Open(FileMode mode);
	
	// Close image handler
	void Close();

	// Add new directory to CEX image
	void AddDirectory();

	// Add new section to CEX image
	void AddSection(Section *);

	short ImageVersion() const;

	//TODO: friend?
	inline InternalImageVersion GetInternalImageVersion() const { return m_interalImageVersion; }
	inline int GetInternalProgramVersion() const { return 0; }

	// Seal the executable in order to generate a valid CEX image. The sealing
	// process guantees a valid CEX is generated and the object cannot be 
	// altered when commited to disk.
	static const Executable& Seal(Executable&);

private:
	void ValidateImageFormat();
	void CreateNewImage();
	void AlignBounds();
	void CalculateInternalOffsets();
	void CalculateImageSize();
	void CalculateSectionOffsets();
	void CalculateDirectoryOffsets();
};

} // namespace CryExecutable
