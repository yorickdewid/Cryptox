// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/Cry.h>
#include <Cry/ByteOrder.h>
#include <Cry/Types.h>

#include <vector>

#define CHECKPOINT_TAG_1 0xd6
#define CHECKPOINT_TAG_2 0x9b

//TODO:
// - Float
// - Double
// - Long double
// - Unsigned Long double

namespace Cry
{

namespace Detail
{

template<typename IntegerType>
struct DeserializeImpl;

template<>
struct DeserializeImpl<Byte>
{
	template<typename BaseType>
	static Byte Delegate(BaseType& base, typename BaseType::OffsetType idx)
	{
		if (idx == -1) {
			idx = base.Offset();
		}

		base.SetOffset(sizeof(Byte));
		return base.at(idx);
	}
};

template<>
struct DeserializeImpl<Short>
{
	template<typename BaseType>
	static Short Delegate(BaseType& base, typename BaseType::OffsetType idx)
	{
		if (idx == -1) {
			idx = base.Offset();
		}

		Short i = base.at(idx)
			| base.at(idx + 1) << 8;

#if CRY_LITTLE_ENDIAN
		i = BSWAP16(i);
#endif
		base.SetOffset(sizeof(Short));
		return i;
	}
};

template<>
struct DeserializeImpl<Word>
{
	template<typename BaseType>
	static Word Delegate(BaseType& base, typename BaseType::OffsetType idx)
	{
		if (idx == -1) {
			idx = base.Offset();
		}

		Word i = base.at(idx)
			| base.at(idx + 1) << 8
			| base.at(idx + 2) << 16
			| base.at(idx + 3) << 24;

#if CRY_LITTLE_ENDIAN
		i = BSWAP32(i);
#endif
		base.SetOffset(sizeof(Word));
		return i;
	}
};

template<>
struct DeserializeImpl<DoubleWord>
{
	template<typename BaseType>
	static DoubleWord Delegate(BaseType& base, typename BaseType::OffsetType idx)
	{
		if (idx == -1) {
			idx = base.Offset();
		}

		DoubleWord i = 0;
		i |= (DoubleWord)base.at(idx) << 0;
		i |= (DoubleWord)base.at(idx + 1) << 8;
		i |= (DoubleWord)base.at(idx + 2) << 16;
		i |= (DoubleWord)base.at(idx + 3) << 24;
		i |= (DoubleWord)base.at(idx + 4) << 32;
		i |= (DoubleWord)base.at(idx + 5) << 40;
		i |= (DoubleWord)base.at(idx + 6) << 48;
		i |= (DoubleWord)base.at(idx + 7) << 56;

#if CRY_LITTLE_ENDIAN
		i = BSWAP64(i);
#endif
		base.SetOffset(sizeof(DoubleWord));
		return i;
	}
};

} // namespace Detail

namespace Trait
{

template<typename IterType, typename = void>
struct IsIterable : std::false_type
{
};

template<typename IterType>
struct IsIterable<IterType, typename std::enable_if<
	!std::is_same_v<typename std::iterator_traits<typename IterType::iterator>::value_type, void>
>::type> : std::true_type
{
};

template<typename IterType>
inline constexpr bool IsIterable_v = IsIterable<IterType>::value;

} // namespace Trait

struct SerializableContract {};

// Byte container with multiplatform and multi archtitecture
// support. The bytearray can be used to write builtin datatypes
// to a byte stream. The byte stream can be converted back into
// native objects. The class offers serialization methods for common
// tricks including byte reordering and structure assertion.
template<typename VectorType>
class BasicArrayBuffer : public VectorType
{
	static_assert(std::is_base_of_v<SerializableContract, VectorType>, "");
	static_assert(Trait::IsIterable_v<VectorType>, "");
	static_assert(std::is_copy_assignable_v<VectorType>, "");
	static_assert(std::is_move_assignable_v<VectorType>, "");

	constexpr static const unsigned char flag0 = 1 << 0;      // 0000 0001
	constexpr static const unsigned char flagIs64 = 1 << 1;   // 0000 0010
	constexpr static const unsigned char flagIsWin = 1 << 2;  // 0000 0100
	constexpr static const unsigned char flagIsUnix = 1 << 3; // 0000 1000
	constexpr static const unsigned char flagisOSX = 1 << 4;  // 0001 0000
	constexpr static const unsigned char flag5 = 1 << 5;      // 0010 0000
	constexpr static const unsigned char flag6 = 1 << 6;      // 0100 0000
	constexpr static const unsigned char flagIsLE = 1 << 7;   // 1000 0000

public:
	using self_type = VectorType;
	using BaseType = VectorType; //TODO: replace
	using ValueType = typename self_type::value_type;
	using SizeType = typename self_type::size_type;
	using OffsetType = int;

	static_assert(sizeof(ValueType) == sizeof(Byte), "Vector type must be byte size");

public:
	BasicArrayBuffer() = default;
	BasicArrayBuffer(const BasicArrayBuffer& other)
		: BaseType{ other }
		, m_offset{ other.m_offset }
	{
	}
	BasicArrayBuffer(BasicArrayBuffer&& other)
		: BaseType{ std::move(other) }
		, m_offset{ other.m_offset }
	{
	}

	template<typename InputIt>
	inline explicit BasicArrayBuffer(InputIt first, InputIt last)
		: BaseType{ first, last }
	{
	}

	//
	// Assignment operators.
	//

	BasicArrayBuffer& operator=(const BasicArrayBuffer& other)
	{
		if (*this == other) { return (*this); }

		m_offset = other.m_offset;
		BaseType::operator=(other);
		return (*this);
	}
	BasicArrayBuffer& operator=(BasicArrayBuffer&& other)
	{
		if (*this == other) { return (*this); }

		m_offset = other.m_offset;
		BaseType::operator=(std::move(other));
		return (*this);
	}

	//
	// Offset operations.
	//

	enum { AUTO = -1 };

	// Set start offset.
	void SetOffset(OffsetType offset) { m_offset += offset; } //TODO: AppendOffset
	// Set start offset.
	void StartOffset(OffsetType offset) { m_offset = offset; }
	// Get current offset.
	int Offset() const noexcept { return m_offset; }
	// Reset offset to zero.
	void Reset() noexcept { m_offset = 0; }

	BasicArrayBuffer& operator++()
	{
		m_offset++;
		return (*this);
	}
	BasicArrayBuffer& operator++(int)
	{
		BasicArrayBuffer *copy = this;
		m_offset++;
		return (*copy);
	}
	BasicArrayBuffer& operator--()
	{
		m_offset--;
		return (*this);
	}
	BasicArrayBuffer& operator--(int)
	{
		BasicArrayBuffer *copy = this;
		m_offset--;
		return (*copy);
	}

	//
	// Validation and consistency.
	//

	// Set magic value.
	void SetMagic(Byte magic)
	{
		BaseType::push_back(magic);
	}

	// Validate magic value. Returns false if check fails.
	bool ValidateMagic(Byte magic, OffsetType idx = -1)
	{
		if (idx == -1) {
			idx = m_offset;
		}
		m_offset += sizeof(Byte);
		return this->at(idx) == magic;
	}

	// Create a new checkpoint.
	void MakeCheckpoint()
	{
		BaseType::insert(this->cend(), { CHECKPOINT_TAG_1, CHECKPOINT_TAG_2 });
	}

	// TODO: check offset is within bounds
	// Validate checkpoint. Returns false if check fails. Accepts an optional
	// offset to read from and an rollback option. If the checkpoint is not found
	// the offset can be reverted back. This is usefull when detecting a checkpoint
	// without hard faillure.
	bool ValidateCheckpoint(bool rollbackOnFaillure = true, OffsetType idx = -1)
	{
		if (idx == -1) {
			idx = m_offset;
		}
		m_offset += (sizeof(Byte) * 2);
		const bool rs = this->at(idx) == CHECKPOINT_TAG_1
			&& this->at(idx + 1) == CHECKPOINT_TAG_2;
		if (!rs && rollbackOnFaillure) {
			m_offset = idx;
		}
		return rs;
	}

	// Encode platform characteristics.
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

	// Check if current platform is compatible.
	// Returns false if check fails.
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
	IntegerType Deserialize(OffsetType idx = -1)
	{
		return Detail::DeserializeImpl<IntegerType>::Delegate(*this, idx);
	}

private:
	OffsetType m_offset{ 0 };
};

template<typename VectorType>
inline bool BasicArrayBuffer<VectorType>::IsPlatformCompat()
{
	const Byte flags = Deserialize<Byte>();
#ifdef CRY_ARCH64
	if (!(flags & flagIs64)) { return false; }
#endif // CRY_ARCH64
#ifdef CRY_WINDOWS
	if (!(flags & flagIsWin)) { return false; }
#endif // CRY_WINDOWS
#ifdef CRY_UNIX
	if (!(flags & flagIsUnix)) { return false; }
#endif // CRY_UNIX
#ifdef CRY_OSX
	if (!(flags & flagisOSX)) { return false; }
#endif // CRY_OSX
#ifdef CRY_LITTLE_ENDIAN
	if (!(flags & flagIsLE)) { return false; }
#endif // CRY_LITTLE_ENDIAN
	return true;
}

template<template<typename Type, typename Allocator = std::allocator<Type>> typename ContainerType, typename ByteType = Byte>
using ByteArrayBuffer = BasicArrayBuffer<ContainerType<std::decay_t<ByteType>>>;

template<typename Type, typename Allocator = std::allocator<Type>>
class VectorBuffer : public std::vector<Type, Allocator>, public SerializableContract
{
	using Base = std::vector<Type, Allocator>;
	using ByteType = Type;
};

// Default byte array type.
using ByteArray = ByteArrayBuffer<VectorBuffer>;

} // namespace Cry
