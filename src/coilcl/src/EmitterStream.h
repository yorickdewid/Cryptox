// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <iostream>

namespace CoilCl
{
namespace Emit
{
namespace Stream
{

// Stream input contract
class InputStream
{
	virtual void Read(std::vector<uint8_t>& vector) {}

public:
	void Read(uint8_t *vector, size_t sz) {}
};

// Stream output contract
class OutputStream
{
	virtual void Write(std::vector<uint8_t>& vector) {}

public:
	void Write(uint8_t *vector, size_t sz) {}
};

// Interact with the console
class Console
	: public InputStream
	, public OutputStream
{
public:
	// Write data stream to console output
	virtual void Write(std::vector<uint8_t>& vector) override
	{
		for (auto& byte : vector) {
			std::cout << std::hex << byte;
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
