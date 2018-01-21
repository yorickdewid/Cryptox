// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be
// copied and/or distributed without the express of the author.

#include "Executable.h"
#include "ExampleSection.h"

using namespace CryExe;

ExampleSection::ExampleSection()
	: Section{ Section::SectionType::NOTE }
{
}

void ExampleSection::DataSwap(DataSwapDirection direction)
{
	if (direction == DataSwapDirection::DATA_SWAP_OUT) {
		// Encode
	}
	else {
		// Decode
	}
}

void ExampleSection::Clear()
{
	Section::Clear();
}
