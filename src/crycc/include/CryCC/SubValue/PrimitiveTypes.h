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

enum class PrimitiveSpecifier
{
    PS_VOID = 200,
    PS_BOOL,
    PS_CHAR,
    PS_SIGNED_CHAR,
    PS_UNSIGNED_CHAR,
    PS_SHORT,
    PS_UNSIGNED_SHORT,
    PS_INT,
    PS_UNSIGNED_INT,
    PS_LONG,
    PS_UNSIGNED_LONG,
    PS_FLOAT,
    PS_DOUBLE,
    PS_LONG_DOUBLE,
    PS_UNSIGNED_LONG_DOUBLE,
};

template<PrimitiveSpecifier Specifier, typename TypeAlias, typename StorageType>
struct PrimitiveType
{
    static_assert(sizeof(StorageType) <= 8, "independent platform overflow");
    static_assert(std::is_fundamental<StorageType>::value, "storage type must be fundamental");

    using storage_type = StorageType;
	using alias = typename std::remove_reference<typename std::remove_cv<TypeAlias>::type>::type;
    static const PrimitiveSpecifier specifier = Specifier;
	static const bool is_unsigned = std::is_unsigned<TypeAlias>::value;
    static const int bit_count = sizeof(StorageType) * BITS_PER_BYTE;
};

using VoidType = PrimitiveType<PrimitiveSpecifier::PS_VOID, void, nullptr_t>;
using BoolType = PrimitiveType<PrimitiveSpecifier::PS_BOOL, bool, bool>;
using CharType = PrimitiveType<PrimitiveSpecifier::PS_CHAR, char, int8_t>;
using SignedCharType = PrimitiveType<PrimitiveSpecifier::PS_SIGNED_CHAR, signed char, int8_t>;
using UnsignedCharType = PrimitiveType<PrimitiveSpecifier::PS_UNSIGNED_CHAR, unsigned char, uint8_t>;
using ShortType = PrimitiveType<PrimitiveSpecifier::PS_SHORT, short, int16_t>;
using UnsignedShortType = PrimitiveType<PrimitiveSpecifier::PS_UNSIGNED_SHORT, unsigned short, uint16_t>;
using IntegerType = PrimitiveType<PrimitiveSpecifier::PS_INT, int, int32_t>;
using UnsignedIntegerType = PrimitiveType<PrimitiveSpecifier::PS_UNSIGNED_INT, unsigned int, uint32_t>;
using LongType = PrimitiveType<PrimitiveSpecifier::PS_LONG, long, int64_t>;
using UnsignedLongType = PrimitiveType<PrimitiveSpecifier::PS_UNSIGNED_LONG, unsigned long, uint64_t>;
using FloatType = PrimitiveType<PrimitiveSpecifier::PS_FLOAT, float, float>;
using DoubleType = PrimitiveType<PrimitiveSpecifier::PS_DOUBLE, double, double>;
using LongDoubleType = PrimitiveType<PrimitiveSpecifier::PS_LONG_DOUBLE, long double, long double>;
using UnsignedLongDoubleType = PrimitiveType<PrimitiveSpecifier::PS_UNSIGNED_LONG_DOUBLE, unsigned long double, unsigned long double>;

static_assert(FloatType::bit_count == 32, "requires set number of bits");
static_assert(DoubleType::bit_count == 64, "requires set number of bits");
static_assert(LongDoubleType::bit_count == 64, "requires set number of bits");
static_assert(UnsignedLongDoubleType::bit_count == 64, "requires set number of bits");

} // namespace Typedef
} // namespace SubValue
} // namespace CryCC
