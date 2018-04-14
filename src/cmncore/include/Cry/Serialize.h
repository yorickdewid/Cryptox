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
using Word = uint32_t;

class ByteArray : public std::vector<Byte>
{
	using _MyBase = std::vector<Byte>;

public:
	ByteArray() = default;

	template<typename _InputIt>
	ByteArray(_InputIt first, _InputIt last)
		: _MyBase{ first, last }
	{
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
	_Ty Deserialize(size_t idx);

	template<>
	uint16_t Deserialize(size_t idx)
	{
		uint16_t i = at(idx)
			| at(idx + 1) << 8;

#if CRY_LITTLE_ENDIAN
		i = BSWAP16(i);
#endif
		return i;
	}

	template<>
	uint32_t Deserialize(size_t idx)
	{
		uint32_t i = at(idx)
			| at(idx + 1) << 8
			| at(idx + 2) << 16
			| at(idx + 3) << 24;

#if CRY_LITTLE_ENDIAN
		i = BSWAP32(i);
#endif
		return i;
	}

	template<>
	uint64_t Deserialize(size_t idx)
	{
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
		return i;
	}
};

} // namespace Cry
