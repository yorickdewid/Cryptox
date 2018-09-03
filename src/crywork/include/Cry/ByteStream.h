// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/Cry.h>
#include <Cry/Serialize.h>

#include <fstream>
#include <string>
#include <bitset>

//TODO:
// - Use primitive types
// - string
// - vectors
// - iterators

namespace Cry
{

namespace Detail
{

#if 0
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
#endif

////

class StreamIOBase
{
	ByteArray m_streambuffer;

protected:
	int m_opts;

public:
	using base_type = ByteArray;
	using value_type = typename base_type::BaseType::value_type;
	using position_type = typename base_type::OffsetType;
	using offset_type = typename base_type::OffsetType;

	enum FlagType
	{
		FLAG_NONE = 0x0,
		FLAG_PLATFORM_CHECK = 0x01,
		FLAG_AUTO_CHECKPOINT = 0x02,
		FLAG_THROW_EXCEPTION = 0x04,
	};

	static constexpr FlagType ClearOptions = FLAG_NONE;
	static constexpr FlagType PlatformCheck = FLAG_PLATFORM_CHECK;
	static constexpr FlagType Checkpoint = FLAG_AUTO_CHECKPOINT;
	static constexpr FlagType CanThrow = FLAG_THROW_EXCEPTION;

	// Return the offset in the stream.
	position_type Tell() const { return m_streambuffer.Offset(); }
	bool Empty() const noexcept { return m_streambuffer.empty(); }
	size_t Size() const noexcept { return m_streambuffer.size(); }

	void SetFlag(FlagType f) { m_opts |= f; }
	void UnSetFlag(FlagType f) { m_opts &= ~f; }

	// Get the byte array from the stream base.
	base_type& Buffer()
	{
		return m_streambuffer;
	}
};

} // namespace Detail

// Stream native data in the byte array.
class ByteInStream : virtual public Detail::StreamIOBase
{
	//

public:
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
	ByteInStream& operator>>(FlagType);

	ByteInStream& Seek(position_type pos)
	{
		Buffer().StartOffset(pos);
		return (*this);
	}

	template<typename NativeType>
	ByteInStream& Get(NativeType& s)
	{
		this->operator>>(s);
		return (*this);
	}

	template<typename NativeType, typename StreamSizeType>
	ByteInStream& Read(NativeType *s, StreamSizeType count)
	{
		for (StreamSizeType i = 0; i < count; ++i) {
			this->operator>>((*s)[i]);
		}
		return (*this);
	}
};

// Stream native data out of the byte array.
class ByteOutStream : virtual public Detail::StreamIOBase
{
	//

public:
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
	ByteOutStream& operator<<(FlagType);

	ByteOutStream& Seek(position_type pos)
	{
		Buffer().StartOffset(pos);
		return (*this);
	}

	template<typename NativeType>
	ByteOutStream& Put(NativeType s)
	{
		this->operator<<(s);
		return (*this);
	}

	template<typename NativeType, typename StreamSizeType>
	ByteOutStream& Write(const NativeType *s, StreamSizeType count)
	{
		for (StreamSizeType i = 0; i < count; ++i) {
			this->operator<<(s[i]);
		}
		return (*this);
	}
};

// Combined in and out stream.
class ByteStream
	: public ByteInStream
	, public ByteOutStream
{
public:
	//
};

#if 0
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
#endif

} // namespace Cry
