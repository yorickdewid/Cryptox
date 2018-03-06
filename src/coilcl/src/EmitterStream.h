// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <iostream>
#include <iomanip>

namespace CoilCl
{
namespace Emit
{
namespace Stream
{

// Stream input contract
class InputStream
{
	virtual void Read(std::vector<uint8_t>& vector)
	{
		CRY_UNUSED(vector);
	}

public:
	void Read(uint8_t *vector, size_t sz)
	{
		CRY_UNUSED(vector);
		CRY_UNUSED(sz);
	}
};

// Stream output contract
class OutputStream
{
	virtual void Write(std::vector<uint8_t>& vector)
	{
		CRY_UNUSED(vector);
	}

public:
	virtual void Write(uint8_t *vector, size_t sz)
	{
		CRY_UNUSED(vector);
		CRY_UNUSED(sz);
	}
};

// Interact with the console
class Console
	: public InputStream
	, public OutputStream
{
public:
	// Write data stream to console output
	virtual void Write(uint8_t *vector, size_t sz) override
	{
		for (size_t i = 0; i < sz; i++)
		{
			const int ch = static_cast<int>(static_cast<unsigned char>(vector[i]));
			std::cout << std::hex << std::setfill('0') << std::setw(2) << ch;
		}
		std::cout << std::flush;
	}
};

// Write or read data to file
class File
	: public InputStream
	, public OutputStream
{
};

// Write or read data from memory slab
class MemoryBlock
	: public InputStream
	, public OutputStream
{
};

} // namespace Stream
} // namespace Emit
} // namespace CoilCl
