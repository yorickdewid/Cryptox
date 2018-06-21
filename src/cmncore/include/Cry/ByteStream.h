// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Cry.h"
#include "Serialize.h"

#include <fstream>
#include <string>

//TODO:
// - string
// - vectors
// - custom objects
// - iterators

namespace Cry
{

#if 0
namespace Detail
{

struct BasicIOBuffer {};

class ByteArrayBuffer
	: public BasicIOBuffer
{
	ByteArray ba;

public:
	ByteArrayBuffer() = default;
};

class FileWriteBuffer
	: public BasicIOBuffer
{
	ByteArray ba;
	std::fstream m_stream;

public:
	FileWriteBuffer() = default;
};

}
#endif

class ByteIOBase : public ByteArray
{
public:
	using Type = ByteArray::BaseType::value_type;
	using PositionType = ByteArray::OffsetType;
	using OffsetType = ByteArray::OffsetType;

	PositionType Tell() const { return this->Offset(); }

	ByteArray Buffer()
	{
		return dynamic_cast<ByteArray&>(*this);
	}
};

class ByteInStream : virtual public ByteIOBase
{
	//

public:
	using Type = ByteArray::BaseType::value_type;
	using PositionType = ByteArray::OffsetType;
	using OffsetType = ByteArray::OffsetType;

	ByteInStream& operator>>(short&);
	ByteInStream& operator>>(unsigned short&);
	ByteInStream& operator>>(int&);
	ByteInStream& operator>>(unsigned int&);
	ByteInStream& operator>>(long&);
	ByteInStream& operator>>(unsigned long&);
	ByteInStream& operator>>(long long&);
	ByteInStream& operator>>(unsigned long long&);
	ByteInStream& operator>>(float&);
	ByteInStream& operator>>(double&);
	ByteInStream& operator>>(long double&);
	ByteInStream& operator>>(bool&);
	ByteInStream& operator>>(ByteInStream&);

	ByteInStream& Seek(PositionType pos)
	{
		this->StartOffset(pos);
		return (*this);
	}

	template<typename NativeType>
	ByteInStream& Get(NativeType& s)
	{
		this->operator>>(s);
		return (*this);
	}

	template<typename NativeType>
	ByteInStream& Read(NativeType *s, std::streamsize count)
	{
		for (std::streamsize i = 0; i < count; ++i) {
			this->operator>>((*s)[i]);
		}
		return (*this);
	}
};

class ByteOutStream : virtual public ByteIOBase
{
	//

public:
	using Type = ByteArray::BaseType::value_type;
	using PositionType = ByteArray::OffsetType;
	using OffsetType = ByteArray::OffsetType;

	ByteOutStream& operator<<(short);
	ByteOutStream& operator<<(unsigned short);
	ByteOutStream& operator<<(int);
	ByteOutStream& operator<<(unsigned int);
	ByteOutStream& operator<<(long);
	ByteOutStream& operator<<(unsigned long);
	ByteOutStream& operator<<(long long);
	ByteOutStream& operator<<(unsigned long long);
	ByteOutStream& operator<<(float);
	ByteOutStream& operator<<(double);
	ByteOutStream& operator<<(long double);
	ByteOutStream& operator<<(bool);
	ByteOutStream& operator<<(ByteOutStream);

	ByteOutStream& Seek(PositionType pos)
	{
		this->StartOffset(pos);
		return (*this);
	}

	template<typename NativeType>
	ByteOutStream& Put(NativeType s)
	{
		this->operator<<(s);
		return (*this);
	}

	template<typename NativeType>
	ByteOutStream& Write(const NativeType *s, std::streamsize count)
	{
		for (std::streamsize i = 0; i < count; ++i) {
			this->operator<<(s[i]);
		}
		return (*this);
	}
};

class ByteStream
	: public ByteInStream
	, public ByteOutStream
{
public:
	//
};

class FileStream
	: public ByteInStream
	, public ByteOutStream
{
public:
	FileStream(const char *filename);
	FileStream(const std::string filename);

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

} // namespace Cry
