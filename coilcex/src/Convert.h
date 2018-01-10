// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <vector>

namespace CryExe
{

class Convert
{
public:
	enum class Operations
	{
		CO_NONE = 1 << 0,
		CO_ENCRYPT = 1 << 1,
		CO_COMPRESS = 1 << 2,
	};

	friend Operations operator|=(Operations op1, Operations op2)
	{
		return static_cast<Operations>(static_cast<int>(op1) | static_cast<int>(op2));
	}

public:
	Convert(const std::vector<uint8_t>& inData, Operations operations)
	{
	}

	void SetProcessOptions()
	{
		//
	}

	void ToImage()
	{
		//
	}

	void FromImage()
	{
		//
	}
};

} // namespace CryExe
