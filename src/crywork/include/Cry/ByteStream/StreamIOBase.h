// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/Cry.h>
#include <Cry/Types.h>
#include <Cry/Serialize.h>

namespace Cry::ByteStream::Detail
{

// Base class for all streaming objects. Provide basics
// such as ownership of the container, container capacity
// methods and stream bookkeeping.
template<template<typename Type, typename Allocator = std::allocator<Type>> typename ContainerType>
class StreamIOBase
{
	ByteArrayBuffer<ContainerType> m_streambuffer;

protected:
	int m_opts{ 0 };

public:
	using base_type = ByteArrayBuffer<ContainerType>;
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

	constexpr static const FlagType ClearOptions = FLAG_NONE;
	constexpr static const FlagType PlatformCheck = FLAG_PLATFORM_CHECK;
	constexpr static const FlagType Checkpoint = FLAG_AUTO_CHECKPOINT;
	constexpr static const FlagType CanThrow = FLAG_THROW_EXCEPTION;

	// Return the offset in the stream.
	position_type Tell() const { return m_streambuffer.Offset(); }
	bool Empty() const noexcept { return m_streambuffer.empty(); }
	size_t Size() const noexcept { return m_streambuffer.size(); }

	void SetFlag(FlagType f) { m_opts |= f; }
	void UnSetFlag(FlagType f) { m_opts &= ~f; }

protected:
	// Get the byte array from the stream base.
	inline base_type& Buffer() noexcept { return m_streambuffer; }
};

} // namespace Cry::ByteStream::Detail
