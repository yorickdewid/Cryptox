// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Notes.h"

#include <iostream>
#include <sstream>

using namespace CryExe;

std::string Notes::ParseNote()
{
	std::stringstream ss;

	ss << "Displaying notes found at file offset 0x00000254 with length 0x00000020:\n";
	ss << "  Owner                Data size       Description\n";
	ss << "  CRY                  0x00000010      NTELF_CRY_ABI_TAG\n";
	ss << "     OS : Linux, ABI : 2.6.32\n";

	ss << "Displaying notes found at file offset 0x00000274 with length 0x00000024:\n";
	ss << "  Owner                Data size       Description\n";
	ss << "  CRY                  0x00000014      NTELF_CRY_BUILD_ID\n";
	ss << "    Build ID: 01125a8df61b8984ab3f9b7d18dc023eca9ff984\n";

	return ss.str();
}

Notes::Notes(const CryExe::Executable& exec)
	: m_exe{ exec }
{
	std::cout << ParseNote() << std::flush;
}
