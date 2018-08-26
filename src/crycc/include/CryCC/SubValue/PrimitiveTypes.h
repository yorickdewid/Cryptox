// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <type_traits>
#include <cstdint>

#define BITS_PER_BYTE 8

namespace CryCC
{
namespace SubValue
{
namespace Typedef
{

template<typename TypeAlias, typename StorageType>
struct PrimitiveType
{
    static_assert(sizeof(StorageType) <= 8, "independent platform overflow");
    static_assert(std::is_fundamental<StorageType>::value, "storage type must be fundamental");

    using storage_type = StorageType;
	using alias = typename std::remove_reference<typename std::remove_cv<TypeAlias>::type>::type;
	static const bool is_unsigned = std::is_unsigned<TypeAlias>::value;
    static const int bit_count = sizeof(StorageType) * BITS_PER_BYTE;
};

using CharType = PrimitiveType<char, int8_t>;
using SignedCharType = PrimitiveType<signed char, int8_t>;
using UnsignedCharType = PrimitiveType<unsigned char, uint8_t>;
using ShortType = PrimitiveType<short, int16_t>;
using UnsignedShortType = PrimitiveType<unsigned short, uint16_t>;
using IntegerType = PrimitiveType<int, int32_t>;
using UnsignedIntegerType = PrimitiveType<unsigned int, uint32_t>;
using LongType = PrimitiveType<long, int64_t>;
using UnsignedLongType = PrimitiveType<unsigned long, uint64_t>;
using FloatType = PrimitiveType<float, float>;
using DoubleType = PrimitiveType<double, double>;
using LongDoubleType = PrimitiveType<long double, long double>;
using UnsignedLongDoubleType = PrimitiveType<unsigned long double, unsigned long double>;

static_assert(FloatType::bit_count == 32, "requires set number of bits");
static_assert(DoubleType::bit_count == 64, "requires set number of bits");
static_assert(LongDoubleType::bit_count == 64, "requires set number of bits");
static_assert(UnsignedLongDoubleType::bit_count == 64, "requires set number of bits");

} // namespace Typedef
} // namespace SubValue
} // namespace CryCC
