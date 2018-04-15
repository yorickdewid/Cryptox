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

// Byte container with multiplatform and multi archtitecture
// support. The bytearray can be used to write builtin datatypes
// to a byte stream. The byte stream can be converted back into
// native objects. The class offers serialization methods for common
// tricks including byte reordering and structure assertion.
class ByteArray : public std::vector<Byte>
{
	using _MyBase = std::vector<Byte>;

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
	ByteArray(const ByteArray& other) { m_offset = other.m_offset; }
	ByteArray(ByteArray&& other) { m_offset = other.m_offset; }

	template<typename _InputIt>
	ByteArray(_InputIt first, _InputIt last)
		: _MyBase{ first, last }
	{
	}

	ByteArray& operator=(const ByteArray& other)
	{
		m_offset = other.m_offset;
		return (*this);
	}
	ByteArray& operator=(ByteArray&& other)
	{
		m_offset = other.m_offset;
		return (*this);
	}

	enum { AUTO = -1 };

	void StartOffset(int offset)
	{
		m_offset = offset;
	}

	int Offset() const noexcept
	{
		return m_offset;
	}

	void SetMagic(uint8_t magic)
	{
		_MyBase::push_back(magic);
	}

	bool ValidateMagic(uint8_t magic, int idx = -1)
	{
		if (idx == -1) {
			idx = m_offset;
		}
		m_offset += sizeof(uint8_t);
		return at(idx) == magic;
	}

	void SetPlatformCompat()
	{
		uint8_t flags = 0;
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

	bool IsPlatformCompat()
	{
		//FUTURE: Do something with flags
		Deserialize<uint8_t>(AUTO);
		return true;
	}

	void Serialize(uint8_t i)
	{
		_MyBase::push_back(i);
	}

	void Serialize(uint16_t i)
	{
#if CRY_LITTLE_ENDIAN
		i = BSWAP16(i);
#endif
		_MyBase::push_back((i >> 0) & 0xff);
		_MyBase::push_back((i >> 8) & 0xff);
	}

	void Serialize(uint32_t i)
	{
#if CRY_LITTLE_ENDIAN
		i = BSWAP32(i);
#endif
		_MyBase::push_back((i >> 0) & 0xff);
		_MyBase::push_back((i >> 8) & 0xff);
		_MyBase::push_back((i >> 16) & 0xff);
		_MyBase::push_back((i >> 24) & 0xff);
	}

	void Serialize(uint64_t i)
	{
#if CRY_LITTLE_ENDIAN
		i = BSWAP64(i);
#endif
		_MyBase::push_back((i >> 0) & 0xff);
		_MyBase::push_back((i >> 8) & 0xff);
		_MyBase::push_back((i >> 16) & 0xff);
		_MyBase::push_back((i >> 24) & 0xff);
		_MyBase::push_back((i >> 32) & 0xff);
		_MyBase::push_back((i >> 40) & 0xff);
		_MyBase::push_back((i >> 48) & 0xff);
		_MyBase::push_back((i >> 56) & 0xff);
	}

	template<typename _Ty>
	void SerializeAs(_Ty i)
	{
		Serialize(static_cast<_Ty>(i));
	}

	template<typename _Ty>
	_Ty Deserialize(int idx);

	template<>
	uint8_t Deserialize(int idx)
	{
		if (idx == -1) {
			idx = m_offset;
		}

		m_offset += sizeof(uint8_t);
		return at(idx);
	}

	template<>
	uint16_t Deserialize(int idx)
	{
		if (idx == -1) {
			idx = m_offset;
		}

		uint16_t i = at(idx)
			| at(idx + 1) << 8;

#if CRY_LITTLE_ENDIAN
		i = BSWAP16(i);
#endif
		m_offset += sizeof(uint16_t);
		return i;
	}

	template<>
	uint32_t Deserialize(int idx)
	{
		if (idx == -1) {
			idx = m_offset;
		}

		uint32_t i = at(idx)
			| at(idx + 1) << 8
			| at(idx + 2) << 16
			| at(idx + 3) << 24;

#if CRY_LITTLE_ENDIAN
		i = BSWAP32(i);
#endif
		m_offset += sizeof(uint32_t);
		return i;
	}

	template<>
	uint64_t Deserialize(int idx)
	{
		if (idx == -1) {
			idx = m_offset;
		}

		uint64_t i = 0;
		i |= (uint64_t)at(idx) << 0;
		i |= (uint64_t)at(idx + 1) << 8;
		i |= (uint64_t)at(idx + 2) << 16;
		i |= (uint64_t)at(idx + 3) << 24;
		i |= (uint64_t)at(idx + 4) << 32;
		i |= (uint64_t)at(idx + 5) << 40;
		i |= (uint64_t)at(idx + 6) << 48;
		i |= (uint64_t)at(idx + 7) << 56;

#if CRY_LITTLE_ENDIAN
		i = BSWAP64(i);
#endif
		m_offset += sizeof(uint64_t);
		return i;
	}

private:
	int m_offset = 0;
};

} // namespace Cry
