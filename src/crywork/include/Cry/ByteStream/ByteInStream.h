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

// Stream native data in the byte array.
template<template<typename Type, typename Allocator = std::allocator<Type>> typename ContainerType = VectorBuffer>
class ByteInStream : virtual public Detail::StreamIOBase<ContainerType>
{
	using base_type = Detail::StreamIOBase<ContainerType>;

	template<typename Type, typename ReinType = nullptr_t>
	auto StreamRead()
	{
		auto value = Buffer().Deserialize<Type::serialize_type>();
		if constexpr (std::is_same_v<ReinType, nullptr_t>) {
			return static_cast<typename Type::alias>(value);
		}
		else {
			return static_cast<typename Type::alias>(reinterpret_cast<ReinType&>(value));
		}
	}

public:
	ByteInStream& operator>>(CharType::alias& v)
	{
		v = StreamRead<CharType>();
		return (*this);
	}
	ByteInStream& operator>>(UnsignedCharType::alias& v)
	{
		v = StreamRead<UnsignedCharType>();
		return (*this);
	}
	ByteInStream& operator>>(ShortType::alias& v)
	{
		v = StreamRead<ShortType>();
		return (*this);
	}
	ByteInStream& operator>>(UnsignedShortType::alias& v)
	{
		v = StreamRead<UnsignedShortType>();
		return (*this);
	}
	ByteInStream& operator>>(IntegerType::alias& v)
	{
		v = StreamRead<IntegerType>();
		return (*this);
	}
	ByteInStream& operator>>(UnsignedIntegerType::alias& v)
	{
		v = StreamRead<UnsignedIntegerType>();
		return (*this);
	}
	ByteInStream& operator>>(LongType::alias& v)
	{
		v = StreamRead<LongType>();
		return (*this);
	}
	ByteInStream& operator>>(UnsignedLongType::alias& v)
	{
		v = StreamRead<UnsignedLongType>();
		return (*this);
	}
	ByteInStream& operator>>(LongLongType::alias& v)
	{
		v = StreamRead<LongLongType>();
		return (*this);
	}
	ByteInStream& operator>>(UnsignedLongLongType::alias& v)
	{
		v = StreamRead<UnsignedLongLongType>();
		return (*this);
	}
	ByteInStream& operator>>(FloatType::alias& v)
	{
		v = StreamRead<FloatType, float>();
		return (*this);
	}
	ByteInStream& operator>>(DoubleType::alias& v)
	{
		v = StreamRead<DoubleType, double>();
		return (*this);
	}
	ByteInStream& operator>>(LongDoubleType::alias& v)
	{
		v = StreamRead<LongDoubleType, double>();
		return (*this);
	}
	ByteInStream& operator>>(BoolType::alias& v)
	{
		v = StreamRead<BoolType>();
		return (*this);
	}
	ByteInStream& operator>>(std::byte& v)
	{
		//FUTURE: remove temporary variable.
		unsigned char va;
		this->operator>>(va);
		v = static_cast<std::byte>(va);
		return (*this);
	}
	ByteInStream& operator>>(std::string& va)
	{
		const size_t vsz = static_cast<size_t>(Buffer().Deserialize<Word>());
		va.resize(vsz);
		for (auto& v : va) {
			this->operator>>(v);
		}
		return (*this);
	}
	ByteInStream& operator>>(ByteInStream& v)
	{
		const size_t vsz = static_cast<size_t>(Buffer().Deserialize<Word>());
		std::copy(Buffer().begin() + Buffer().Offset(), Buffer().begin() + Buffer().Offset() + vsz, std::back_inserter(v.Buffer()));
		//TODO: fix offset
		return (*this);
	}
	ByteInStream& operator>>(FlagType v)
	{
		SetFlag(v);
		return (*this);
	}

	template<typename Type>
	ByteInStream& operator>>(std::vector<Type>& iterable)
	{
		const size_t vsz = static_cast<size_t>(Buffer().Deserialize<Word>());
		iterable.resize(vsz);
		for (auto& v : iterable) {
			this->operator>>(v);
		}
		return (*this);
	}

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

	inline bool HasStreamBarrier() noexcept
	{
		return Buffer().ValidateCheckpoint();
	}
};

} // namespace Cry::ByteStream
