// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be
// copied and/or distributed without the express of the author.

#include "Executable.h"
#include "SourceSection.h"

using namespace CryExe;

SourceSection::SourceSection()
	: Section{ Section::SectionType::SOURCE }
{
}

void SourceSection::DataSwap(DataSwapDirection direction)
{
	if (direction == DataSwapDirection::DATA_SWAP_OUT) {
		// Encode
	}
	else {
		// Decode
	}
}

void SourceSection::Clear()
{
	Section::Clear();
}
