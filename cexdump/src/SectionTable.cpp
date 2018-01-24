// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <Cry/Indep.h>

#include "cex.h"
#include "SectionTable.h"

#include <boost/format.hpp>

#include <iostream>

using namespace CryExe;

std::string SectionNameConv(Section::SectionType type)
{
	switch (type) {
	case Section::SectionType::NATIVE:
		return ".text";
	case Section::SectionType::RESOURCE:
		return ".rsrc";
	case Section::SectionType::DATA:
		return ".data";
	case Section::SectionType::DEBUG:
		return ".debug";
	case Section::SectionType::SOURCE:
		return ".rc";
	case Section::SectionType::NOTE:
		return ".note";
	default:
		break;
	}

	throw std::exception{};
}

std::string SectionTypeConv(Section::SectionType type)
{
	switch (type) {
	case Section::SectionType::NATIVE:
		return "NATIVE";
	case Section::SectionType::RESOURCE:
		return "RESOURCE";
	case Section::SectionType::DATA:
		return "DATA";
	case Section::SectionType::DEBUG:
		return "DEBUG";
	case Section::SectionType::SOURCE:
		return "SOURCE";
	case Section::SectionType::NOTE:
		return "NOTE";
	default:
		break;
	}

	throw std::exception{};
}

std::string SectionTable::SectionRow(MetaSection section, int number)
{
	boost::format format = boost::format{ "  [%d]  %-15s %-13s %08.d    %016.d  %s" }
		% number
		% section.name
		% section.type
		% section.offset
		% section.size
		% section.flags;

	return boost::format{ format }.str();
}

std::string SectionTable::TableLegend()
{
	return "Key to Flags:\n"
		"  E(encrypt)"
		"  C(compress)\n";
}

SectionTable::SectionTable(const CryExe::Executable& exec)
	: m_exec{ exec }
{
}

bool SectionTable::HasSections() const
{
	return !m_exec.Sections().empty();
}

void SectionTable::ForEach(SectionCallback cb)
{
	MetaSection meta;

	for (const auto& listItem : m_exec.Sections()) {
		meta.name = SectionNameConv(listItem.Type());;
		meta.type = SectionTypeConv(listItem.Type());
		meta.offset = Meta::SectionIntric::ImageOffset(listItem);
		meta.size = Meta::SectionIntric::ImageDataSize(listItem);
		CRY_MEMSET(&meta.flags, '\0', sizeof(meta.flags));

		cb(meta);
	}
}

void SectionTable::ParseTable(const CryExe::Executable& exec)
{
	SectionTable sections{ exec };

	// Display sections only available
	if (!sections.HasSections()) { return; }

	std::cout << "Sections:" << std::endl;
	std::cout << "  [Nr] Name            Type          Offset      Size              Flags" << std::endl;

	int counter = 0;
	sections.ForEach([&sections, &counter](MetaSection& section)
	{
		std::cout << sections.SectionRow(section, ++counter) << std::endl;
	});

	std::cout << TableLegend() << std::flush;
}
