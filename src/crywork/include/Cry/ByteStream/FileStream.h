// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/ByteStream/ByteInStream.h>
#include <Cry/ByteStream/ByteOutStream.h>

#include <fstream>

// FUTURE
//  - Implementation

namespace Cry::ByteStream
{

#if 0

class FileStream
	: public ByteInStream<>
	, public ByteOutStream<>
{
public:
	FileStream(const char *filename)
		: m_stream{ filename }
	{}
	FileStream(const std::string filename)
		: m_stream{ filename }
	{}

	FileStream(FileStream&& other) = default;
	FileStream(const FileStream&) = delete;

	bool IsOpen() { return m_stream.is_open(); }
	void Open(const char *filename) { m_stream.open(filename); }
	void Open(const std::string& filename) { m_stream.open(filename); }
	void Close() { m_stream.close(); }

	bool operator!() { return !IsOpen(); }

private:
	std::fstream m_stream;
};

#endif

} // namespace Cry::ByteStream
