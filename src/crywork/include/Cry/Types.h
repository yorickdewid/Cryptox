// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/Cry.h>

#include <type_traits>
#include <cstdint>
#include <cstddef>

#define BITS_PER_BYTE 8

namespace Cry
{

using Byte = uint8_t;
using Short = uint16_t; //TODO -> int16_t
using Word = uint32_t; //TODO -> int32_t
using DoubleWord = uint64_t; //TODO -> int64_t
using UShort = std::make_unsigned_t<Short>;
using UWord = std::make_unsigned_t<DoubleWord>;
using UDoubleWord = std::make_unsigned_t<DoubleWord>;

enum class PrimitiveSpecifier
{
	PS_INVAL = 0,
	PS_RESV1 = 101,
	PS_RESV2 = 102,
	PS_RESV3 = 103,
	PS_RESV4 = 104,
	PS_RESV5 = 105,
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

template<PrimitiveSpecifier Specifier, typename TypeAlias, typename StorageType, typename SerializeType = StorageType>
struct PrimitiveType
{
	static_assert(sizeof(StorageType) <= 8, "independent platform overflow");
	static_assert(std::is_fundamental<StorageType>::value, "storage type must be fundamental");

	using storage_type = StorageType;
	using serialize_type = std::make_unsigned_t<SerializeType>;
	using alias = typename std::remove_reference<typename std::remove_cv<TypeAlias>::type>::type;
	static const PrimitiveSpecifier specifier = Specifier;
	static const bool is_unsigned = std::is_unsigned<TypeAlias>::value;
	static const int bit_count = sizeof(serialize_type) * BITS_PER_BYTE;
};

using VoidType = PrimitiveType<PrimitiveSpecifier::PS_VOID, void, nullptr_t>;
using BoolType = PrimitiveType<PrimitiveSpecifier::PS_BOOL, bool, bool, int8_t>;
using CharType = PrimitiveType<PrimitiveSpecifier::PS_CHAR, char, int8_t>;
using SignedCharType = PrimitiveType<PrimitiveSpecifier::PS_SIGNED_CHAR, signed char, int8_t>;
using UnsignedCharType = PrimitiveType<PrimitiveSpecifier::PS_UNSIGNED_CHAR, unsigned char, uint8_t>;
using ShortType = PrimitiveType<PrimitiveSpecifier::PS_SHORT, short, int16_t>;
using UnsignedShortType = PrimitiveType<PrimitiveSpecifier::PS_UNSIGNED_SHORT, unsigned short, uint16_t>;
using IntegerType = PrimitiveType<PrimitiveSpecifier::PS_INT, int, int32_t>;
using UnsignedIntegerType = PrimitiveType<PrimitiveSpecifier::PS_UNSIGNED_INT, unsigned int, uint32_t>;
using LongType = PrimitiveType<PrimitiveSpecifier::PS_LONG, long, int64_t>;
using UnsignedLongType = PrimitiveType<PrimitiveSpecifier::PS_UNSIGNED_LONG, unsigned long, uint64_t>;
using FloatType = PrimitiveType<PrimitiveSpecifier::PS_FLOAT, float, float, int32_t>;
using DoubleType = PrimitiveType<PrimitiveSpecifier::PS_DOUBLE, double, double, int64_t>;
using LongDoubleType = PrimitiveType<PrimitiveSpecifier::PS_LONG_DOUBLE, long double, long double, int64_t>;
using UnsignedLongDoubleType = PrimitiveType<PrimitiveSpecifier::PS_UNSIGNED_LONG_DOUBLE, unsigned long double, unsigned long double, int64_t>;

static_assert(FloatType::bit_count == 32, "requires set number of bits");
static_assert(DoubleType::bit_count == 64, "requires set number of bits");
static_assert(LongDoubleType::bit_count == 64, "requires set number of bits");
static_assert(UnsignedLongDoubleType::bit_count == 64, "requires set number of bits");

namespace Detail
{

enum Inval {};

template<typename Type>
class PrimitiveSelectorImpl
{
	template<typename ValueType>
	constexpr static auto Test(ValueType)->Inval;
	template<typename>
	constexpr static auto Test(BoolType::alias)->BoolType;
	template<typename>
	constexpr static auto Test(CharType::alias)->CharType;
	template<typename>
	constexpr static auto Test(SignedCharType::alias)->SignedCharType;
	template<typename>
	constexpr static auto Test(UnsignedCharType::alias)->UnsignedCharType;
	template<typename>
	constexpr static auto Test(ShortType::alias)->ShortType;
	template<typename>
	constexpr static auto Test(UnsignedShortType::alias)->UnsignedShortType;
	template<typename>
	constexpr static auto Test(IntegerType::alias)->IntegerType;
	template<typename>
	constexpr static auto Test(UnsignedIntegerType::alias)->UnsignedIntegerType;
	template<typename>
	constexpr static auto Test(LongType::alias)->LongType;
	template<typename>
	constexpr static auto Test(UnsignedLongType::alias)->UnsignedLongType;
	template<typename>
	constexpr static auto Test(FloatType::alias)->FloatType;
	template<typename>
	constexpr static auto Test(DoubleType::alias)->DoubleType;
	template<typename>
	constexpr static auto Test(LongDoubleType::alias)->LongDoubleType;
	template<typename>
	constexpr static auto Test(UnsignedLongDoubleType::alias)->UnsignedLongDoubleType;

public:
	using type = decltype(Test<Type>(Type{}));
};

} // namespace Detail

template<typename Type>
struct PrimitiveSelector : public Detail::PrimitiveSelectorImpl<Type>::type
{
	using self_type = typename Detail::PrimitiveSelectorImpl<Type>::type;
	using type = typename self_type::storage_type;
};

template<typename Type>
using PrimitiveSelectorStorageType = typename PrimitiveSelector<Type>::storage_type;

template<typename Type>
struct IsPrimitiveType
{
	inline constexpr static const bool value = std::negation<
		std::is_same<typename Detail::PrimitiveSelectorImpl<Type>::type, Detail::Inval>
	>::value;
};

template<typename Type>
inline constexpr bool IsPrimitiveType_v = IsPrimitiveType<Type>::value;

static_assert(std::is_same<PrimitiveSelectorStorageType<bool>, bool>::value, "invalid type translation");
static_assert(std::is_same<PrimitiveSelectorStorageType<char>, int8_t>::value, "invalid type translation");
static_assert(std::is_same<PrimitiveSelectorStorageType<signed char>, int8_t>::value, "invalid type translation");
static_assert(std::is_same<PrimitiveSelectorStorageType<unsigned char>, uint8_t>::value, "invalid type translation");
static_assert(std::is_same<PrimitiveSelectorStorageType<short>, int16_t>::value, "invalid type translation");
static_assert(std::is_same<PrimitiveSelectorStorageType<unsigned short>, uint16_t>::value, "invalid type translation");
static_assert(std::is_same<PrimitiveSelectorStorageType<int>, int32_t>::value, "invalid type translation");
static_assert(std::is_same<PrimitiveSelectorStorageType<unsigned int>, uint32_t>::value, "invalid type translation");
static_assert(std::is_same<PrimitiveSelectorStorageType<long>, int64_t>::value, "invalid type translation");
static_assert(std::is_same<PrimitiveSelectorStorageType<unsigned long>, uint64_t>::value, "invalid type translation");
static_assert(std::is_same<PrimitiveSelectorStorageType<float>, float>::value, "invalid type translation");
static_assert(std::is_same<PrimitiveSelectorStorageType<double>, double>::value, "invalid type translation");
// static_assert(std::is_same<PrimitiveSelectorStorageType<long double>, long double>::value, "invalid type translation");
// static_assert(std::is_same<PrimitiveSelectorStorageType<unsigned long double>, unsigned long double>::value, "invalid type translation");

} // namespace Cry
