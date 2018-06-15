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

#include <string>

//TODO:
// - string
// - vectors

namespace Cry
{

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

	template<typename Type>
	ByteInStream& Read(Type /**s*/, std::streamsize /*count*/)
	{
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

	template<typename Type>
	ByteOutStream& Put(Type /*ch*/)
	{
		return (*this);
	}

	template<typename Type>
	ByteOutStream& Write(const Type /**s*/, std::streamsize /*count*/)
	{
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
	bool IsOpen() { return false; }
	void Open(const char *filename) {}
	void Open(const std::string& filename) {}
	void Close() {}

	bool operator!() { return !IsOpen(); }
};

} // namespace Cry
