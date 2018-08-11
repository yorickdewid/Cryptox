// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <utility>

namespace CryCC
{
namespace Detail
{

// Source location implementation.
template<typename Type = int, typename = typename std::enable_if<std::is_integral<Type>::value>::type>
struct SourceLocationImpl : private std::pair<Type, Type>
{
	using base_type = std::pair<Type, Type>;
	using value_type = typename base_type::first_type;
	using self_type = SourceLocationImpl<Type>;

	SourceLocationImpl()
		: base_type{ -1, -1 }
	{
	}
	
	inline SourceLocationImpl(const value_type line, const value_type col)
		: base_type{ line, col }
	{
	}

	inline SourceLocationImpl(const std::pair<value_type, value_type>& location)
		: base_type{ location.first, location.second }
	{
	}

	inline SourceLocationImpl(const self_type& other)
		: base_type{ other }
	{
	}

	inline SourceLocationImpl(self_type&& other)
		: base_type{ std::move(other) }
	{
	}

	SourceLocationImpl& operator=(const self_type& other)
	{
		this->first = other.first;
		this->second = other.second;
		return (*this);
	}

	SourceLocationImpl& operator=(self_type&& other)
	{
		this->first = std::move(other.first);
		this->second = std::move(other.second);
		return (*this);
	}

	//
	// Access location properties.
	//

	value_type Line() const noexcept { return this->first; }
	value_type Column() const noexcept { return this->second; }

	//
	// Compare operations.
	//

	inline bool operator==(const SourceLocationImpl& other) const noexcept
	{
		return this->first == other.first && this->second == other.second;
	}
	inline bool operator!=(const SourceLocationImpl& other) const noexcept
	{
		return !this->operator==(other);
	}
	inline bool operator<(const SourceLocationImpl& other) const noexcept
	{
		return this->first < other.first || (this->first == other.first && this->second < other.second);
	}
	inline bool operator>(const SourceLocationImpl& other) const noexcept
	{
		return this->first > other.first || (this->first == other.first && this->second > other.second);
	}
	inline bool operator<=(const SourceLocationImpl& other) const noexcept
	{
		return this->operator<(other) || this->operator==(other);
	}
	inline bool operator>=(const SourceLocationImpl& other) const noexcept
	{
		return this->operator>(other) || this->operator==(other);
	}

	//
	// Arithmetic operations.
	//

	SourceLocationImpl operator+(const SourceLocationImpl& other) const noexcept
	{
		return self_type{ this->first + other.first, this->second + other.second };
	}
	SourceLocationImpl operator-(const SourceLocationImpl& other) const noexcept
	{
		return self_type{ this->first - other.first, this->second - other.second };
	}
};

} // namespace Detail

using SourceLocation = Detail::SourceLocationImpl<>;
using SourceLocationLong = Detail::SourceLocationImpl<long long int>;

} // namespace CryCC
