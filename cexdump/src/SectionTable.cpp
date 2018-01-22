// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "SectionTable.h"

#include <iostream>
#include <sstream>

using namespace CryExe;

std::string SectionTable::SectionRow(int number)
{
	std::stringstream ss;

	ss << "  [" << number << "].interp           NOTEBITS      00000238    0000000000000000  A\n";

	return ss.str();
}

std::string SectionTable::TableLegend()
{
	return "Key to Flags:\n"
		"  W(write), A(alloc), X(execute), M(merge), S(strings), I(info),\n"
		"  L(link order), O(extra OS processing required), G(group), T(TLS),\n"
		"  C(compressed), x(unknown), o(OS specific), E(exclude),\n"
		"  l(large), p(processor specific)\n";
}

void SectionTable::ParseTable(const CryExe::Executable& exec)
{
	//const std::string programVersion = Meta::ProgramVersion(exec);
	((void)exec);

	std::cout << "Sections:" << '\n'
		<< "  [Nr] Name            Type          Offset      Size              Flags" << '\n'
		<< SectionTable{}.SectionRow(1)
		<< SectionTable{}.SectionRow(2)
		<< SectionTable{}.SectionRow(3)
		<< TableLegend()
		<< std::flush;
}
