// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "OSAdapter.h"
#include "Executable.h"

namespace CryExe
{

using ByteArray = std::vector<std::uint8_t>;

class COILCEXAPI Directory
{
	friend class Executable;

private:
	ByteArray data;

private:
	struct COILCEXAPI DataPosition
	{
		DataPosition() = default;
		DataPosition(OSFilePosition offset, size_t size)
			: internalImageDataOffset{ offset }
			, internalImageDataSize{ size }
		{
		}

		OSFilePosition internalImageDataOffset = ILLEGAL_OFFSET;
		size_t internalImageDataSize = 0;
	} m_dataPosition;

public:
	Directory() = default;
};

} // namespace CryExe
