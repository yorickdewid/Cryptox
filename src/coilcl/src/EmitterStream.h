// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

namespace CoilCl
{
namespace Emit
{
namespace Stream
{

class InputStream
{
	void Read(std::vector<uint8_t>& vector) {}
	void Read(uint8_t *vector, size_t sz) {}
};

class OutputStream
{
	void Write(std::vector<uint8_t>& vector) {}
	void Write(uint8_t *vector, size_t sz) {}
};

class Console
	: public InputStream
	, public OutputStream
{
};

class File
	: public InputStream
	, public OutputStream
{
};

class MemoryBlock
	: public InputStream
	, public OutputStream
{
};

} // namespace Stream
} // namespace Emit
} // namespace CoilCl
