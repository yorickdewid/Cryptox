// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/ByteStream/StreamIOBase.h>

//FUTURE:
// - string and vector can be optimized

namespace Cry::ByteStream
{

// Stream native data out of the byte array.
template<template<typename Type, typename Allocator = std::allocator<Type>> typename ContainerType = VectorBuffer>
class ByteOutStream : virtual public Detail::StreamIOBase<ContainerType>
{
	using base_type = Detail::StreamIOBase<ContainerType>;

	template<typename Type, typename InputType>
	auto StreamWrite(InputType v)
	{
		Buffer().SerializeAs<Type::serialize_type>(v);
		return (*this);
	}

public:
	ByteOutStream& operator<<(CharType::alias v)
	{
		StreamWrite<CharType>(v);
		return (*this);
	}
	ByteOutStream& operator<<(UnsignedCharType::alias v)
	{
		StreamWrite<UnsignedCharType>(v);
		return (*this);
	}
	ByteOutStream& operator<<(ShortType::alias v)
	{
		StreamWrite<ShortType>(v);
		return (*this);
	}
	ByteOutStream& operator<<(UnsignedShortType::alias v)
	{
		StreamWrite<UnsignedShortType>(v);
		return (*this);
	}
	ByteOutStream& operator<<(IntegerType::alias v)
	{
		StreamWrite<IntegerType>(v);
		return (*this);
	}
	ByteOutStream& operator<<(UnsignedIntegerType::alias v)
	{
		StreamWrite<UnsignedIntegerType>(v);
		return (*this);
	}
	ByteOutStream& operator<<(LongType::alias v)
	{
		StreamWrite<LongType>(v);
		return (*this);
	}
	ByteOutStream& operator<<(UnsignedLongType::alias v)
	{
		StreamWrite<UnsignedLongType>(v);
		return (*this);
	}
	ByteOutStream& operator<<(LongLongType::alias v)
	{
		StreamWrite<LongLongType>(v);
		return (*this);
	}
	ByteOutStream& operator<<(UnsignedLongLongType::alias v)
	{
		StreamWrite<UnsignedLongLongType>(v);
		return (*this);
	}
	ByteOutStream& operator<<(FloatType::alias v)
	{
		StreamWrite<FloatType>(reinterpret_cast<FloatType::serialize_type&>(v));
		return (*this);
	}
	ByteOutStream& operator<<(DoubleType::alias v)
	{
		StreamWrite<DoubleType>(reinterpret_cast<DoubleType::serialize_type&>(v));
		return (*this);
	}
	ByteOutStream& operator<<(LongDoubleType::alias v)
	{
		StreamWrite<LongDoubleType>(reinterpret_cast<DoubleType::serialize_type&>(v));
		return (*this);
	}
	ByteOutStream& operator<<(BoolType::alias v)
	{
		StreamWrite<BoolType>(v);
		return (*this);
	}
	ByteOutStream& operator<<(std::byte v)
	{
		this->operator<<(static_cast<unsigned char>(v));
		return (*this);
	}
	ByteOutStream& operator<<(const std::string& v)
	{
		this->WriteIterator(v.begin(), v.end());
		return (*this);
	}
	ByteOutStream& operator<<(ByteOutStream v)
	{
		Buffer().SerializeAs<Word>(v.Buffer().size());
		Buffer().insert(Buffer().cend(), v.Buffer().cbegin(), v.Buffer().cend());
		return (*this);
	}
	ByteOutStream& operator<<(FlagType v)
	{
		SetFlag(v);
		return (*this);
	}

	template<typename Type>
	ByteOutStream& operator<<(const std::initializer_list<Type>& iterable)
	{
		this->WriteIterator(iterable.begin(), iterable.end());
		return (*this);
	}

	template<typename Type>
	ByteOutStream& operator<<(const std::vector<Type>& iterable)
	{
		this->WriteIterator(iterable.begin(), iterable.end());
		return (*this);
	}

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

	template<typename IterType>
	ByteOutStream& WriteIterator(IterType first, IterType last)
	{
		Buffer().SerializeAs<Word>(std::distance(first, last));
		std::for_each(first, last, [=](auto v) { this->operator<<(v); });
		return (*this);
	}
};

} // namespace Cry::ByteStream
