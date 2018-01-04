// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

//#include "Exportable.h"
#include "Executable.h"

#include <string>
#include <vector>

namespace CryExe
{

using ByteArray = std::vector<std::uint8_t>;

class COILCEXAPI Section
{
	ByteArray data;

public:
	void PushContent(const std::vector<std::uint8_t>& bstream)
	{
		data = bstream;
	}

	void PushContent(std::vector<std::uint8_t>&& bstream)
	{
		data = std::move(bstream);
	}

	// Convert string to byte array
	inline void PushContent(const std::string& sstream)
	{
		std::vector<std::uint8_t> vec{ sstream.begin(), sstream.end() };
		PushContent(std::move(vec));
	}

	ByteArray Data() const { return data; }

	inline bool IsAllowedOnce() const { return true; }
};

} // namespace CryExe
