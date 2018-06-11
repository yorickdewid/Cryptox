// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Cry.h"
#include "ByteOrder.h"

#include <vector>

namespace Cry
{

using Byte = uint8_t;
using Short = uint16_t;
using Word = uint32_t;
using DoubleWord = uint64_t;

// Byte container with multiplatform and multi archtitecture
// support. The bytearray can be used to write builtin datatypes
// to a byte stream. The byte stream can be converted back into
// native objects. The class offers serialization methods for common
// tricks including byte reordering and structure assertion.
class ByteArray : public std::vector<Byte>
{
	using BaseType = std::vector<Byte>;

	const unsigned char flag0 = 1 << 0;      // 0000 0001
	const unsigned char flagIs64 = 1 << 1;   // 0000 0010
	const unsigned char flagIsWin = 1 << 2;  // 0000 0100
	const unsigned char flagIsUnix = 1 << 3; // 0000 1000
	const unsigned char flagisOSX = 1 << 4;  // 0001 0000
	const unsigned char flag5 = 1 << 5;      // 0010 0000
	const unsigned char flag6 = 1 << 6;      // 0100 0000
	const unsigned char flagIsLE = 1 << 7;   // 1000 0000

public:
	ByteArray() = default;
	ByteArray(const ByteArray& other)
		: BaseType{ other }
		, m_offset{ other.m_offset }
	{
	}
	ByteArray(ByteArray&& other)
		: BaseType{ std::move(other) }
		, m_offset{ other.m_offset } //FIXME: this does not work
	{
	}

	template<typename InputIt>
	inline ByteArray(InputIt first, InputIt last)
		: BaseType{ first, last }
	{
	}

	//
	// Assignment operators
	//

	ByteArray& operator=(const ByteArray& other)
	{
		m_offset = other.m_offset;
		BaseType::operator=(other);
		return (*this);
	}
	ByteArray& operator=(ByteArray&& other)
	{
		m_offset = other.m_offset;
		BaseType::operator=(std::move(other));
		return (*this);
	}

	//
	// Offset Operations
	//

	enum { AUTO = -1 };

	// Set start offset
	void SetOffset(int offset) { m_offset += offset; }
	// Set start offset
	void StartOffset(int offset) { m_offset = offset; }
	// Get current offset
	int Offset() const noexcept { return m_offset; }

	// Set magic value
	void SetMagic(Byte magic)
	{
		BaseType::push_back(magic);
	}

	// Validate magic value
	bool ValidateMagic(Byte magic, int idx = -1)
	{
		if (idx == -1) {
			idx = m_offset;
		}
		m_offset += sizeof(Byte);
		return at(idx) == magic;
	}

	// Encode platform characteristics
	void SetPlatformCompat()
	{
		Byte flags = 0;
#ifdef CRY_ARCH64
		flags |= flagIs64;
#endif // CRY_ARCH64
#ifdef CRY_WINDOWS
		flags |= flagIsWin;
#endif // CRY_WINDOWS
#ifdef CRY_UNIX
		flags |= flagIsUnix;
#endif // CRY_UNIX
#ifdef CRY_OSX
		flags |= flagisOSX;
#endif // CRY_OSX
#ifdef CRY_LITTLE_ENDIAN
		flags |= flagIsLE;
#endif // CRY_LITTLE_ENDIAN
		Serialize(flags);
	}

	// Check if current platform is compatible
	bool IsPlatformCompat();

	void Serialize(Byte i)
	{
		BaseType::push_back(i);
	}

	void Serialize(Short i)
	{
#if CRY_LITTLE_ENDIAN
		i = BSWAP16(i);
#endif
		BaseType::push_back((i >> 0) & 0xff);
		BaseType::push_back((i >> 8) & 0xff);
	}

	void Serialize(Word i)
	{
#if CRY_LITTLE_ENDIAN
		i = BSWAP32(i);
#endif
		BaseType::push_back((i >> 0) & 0xff);
		BaseType::push_back((i >> 8) & 0xff);
		BaseType::push_back((i >> 16) & 0xff);
		BaseType::push_back((i >> 24) & 0xff);
	}

	void Serialize(DoubleWord i)
	{
#if CRY_LITTLE_ENDIAN
		i = BSWAP64(i);
#endif
		BaseType::push_back((i >> 0) & 0xff);
		BaseType::push_back((i >> 8) & 0xff);
		BaseType::push_back((i >> 16) & 0xff);
		BaseType::push_back((i >> 24) & 0xff);
		BaseType::push_back((i >> 32) & 0xff);
		BaseType::push_back((i >> 40) & 0xff);
		BaseType::push_back((i >> 48) & 0xff);
		BaseType::push_back((i >> 56) & 0xff);
	}

	template<typename ToConversionType, typename FromIntegerType>
	void SerializeAs(FromIntegerType i)
	{
		Serialize(static_cast<ToConversionType>(i));
	}

	template<typename IntegerType>
	IntegerType Deserialize(int idx = -1);

private:
	int m_offset = 0;
};

template<>
inline Byte ByteArray::Deserialize(int idx)
{
	if (idx == -1) {
		idx = m_offset;
	}

	m_offset += sizeof(Byte);
	return at(idx);
}

template<>
inline Short ByteArray::Deserialize(int idx)
{
	if (idx == -1) {
		idx = m_offset;
	}

	Short i = at(idx)
		| at(idx + 1) << 8;

#if CRY_LITTLE_ENDIAN
	i = BSWAP16(i);
#endif
	m_offset += sizeof(Short);
	return i;
}

template<>
inline Word ByteArray::Deserialize(int idx)
{
	if (idx == -1) {
		idx = m_offset;
	}

	Word i = at(idx)
		| at(idx + 1) << 8
		| at(idx + 2) << 16
		| at(idx + 3) << 24;

#if CRY_LITTLE_ENDIAN
	i = BSWAP32(i);
#endif
	m_offset += sizeof(Word);
	return i;
}

template<>
inline DoubleWord ByteArray::Deserialize(int idx)
{
	if (idx == -1) {
		idx = m_offset;
	}

	DoubleWord i = 0;
	i |= (DoubleWord)at(idx) << 0;
	i |= (DoubleWord)at(idx + 1) << 8;
	i |= (DoubleWord)at(idx + 2) << 16;
	i |= (DoubleWord)at(idx + 3) << 24;
	i |= (DoubleWord)at(idx + 4) << 32;
	i |= (DoubleWord)at(idx + 5) << 40;
	i |= (DoubleWord)at(idx + 6) << 48;
	i |= (DoubleWord)at(idx + 7) << 56;

#if CRY_LITTLE_ENDIAN
	i = BSWAP64(i);
#endif
	m_offset += sizeof(DoubleWord);
	return i;
}

inline bool ByteArray::IsPlatformCompat()
{
	//FUTURE: Do something with flags
	Deserialize<Byte>(AUTO);
	return true;
}

} // namespace Cry
