// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Notes.h"

#include <boost/format.hpp>

#include <iostream>
#include <sstream>

using namespace CryExe;

constexpr static const CryExe::Section::SectionType _section = CryExe::Section::SectionType::NOTE;

std::string Notes::ParseNote(NoteSection& section)
{
	std::stringstream ss;

	const boost::format format = boost::format{ "  %-20s  %016.d   %s" }
		% section.Name()
		% Meta::SectionIntric::ImageDataSize(section)
		% section.Description();

	ss << "Displaying notes found at image offset " << boost::format{ "%08.d" } % Meta::SectionIntric::ImageOffset(section) << ":\n";
	ss << "  Owner                 Data size          Description\n";
	ss << format << "\n";
	ss << "     " << section.Context() << "\n";

	return ss.str();
}

Notes::Notes(const CryExe::Executable& exec)
	: m_exe{ exec }
{
	NoteSection note;

	// Skip if sections are empty
	if (m_exe.Sections().empty()) { return; }

	// Must have at least one note section
	if (const_cast<CryExe::Executable&>(m_exe).FindSection(_section) == m_exe.Sections().end()) { return; }

	//TODO: iterator multiple note sections
	const_cast<CryExe::Executable&>(m_exe).GetSection(&note);

	std::cout << ParseNote(note) << std::flush;
}
