// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/Cry.h>

namespace Cry::Functional
{

template<typename ArgumentType>
struct AllTrue
{
	using argument_type = ArgumentType;
	using result_type = bool;

	constexpr bool operator()(const ArgumentType& value) const
	{
		CRY_UNUSED(value);
		return true;
	}
};

template<typename ArgumentType>
struct AllFalse
{
	using argument_type = ArgumentType;
	using result_type = bool;

	constexpr bool operator()(const ArgumentType& value) const
	{
		CRY_UNUSED(value);
		return false;
	}
};

template<typename LHSType, typename RHSType>
struct Plus
{
	using first_argument_type = LHSType;
	using second_argument_type = RHSType;
	using result_type = decltype(std::declval<LHSType>() + std::declval<RHSType>());

	constexpr auto operator()(const LHSType& lhs, const RHSType& rhs) const -> result_type
	{
		return (lhs + rhs);
	}
};

template<typename Type>
struct Plus<Type, Type>
{
	using first_argument_type = Type;
	using second_argument_type = Type;
	using result_type = decltype(std::declval<Type>() + std::declval<Type>());

	constexpr auto operator()(const Type& lhs, const Type& rhs) const -> result_type
	{
		return (lhs + rhs);
	}
};

template<typename LHSType, typename RHSType>
struct Minus
{
	using first_argument_type = LHSType;
	using second_argument_type = RHSType;
	using result_type = decltype(std::declval<LHSType>() - std::declval<RHSType>());

	constexpr auto operator()(const LHSType& lhs, const RHSType& rhs) const -> result_type
	{
		return (lhs - rhs);
	}
};

template<typename Type>
struct Minus<Type, Type>
{
	using first_argument_type = Type;
	using second_argument_type = Type;
	using result_type = decltype(std::declval<Type>() - std::declval<Type>());

	constexpr auto operator()(const Type& lhs, const Type& rhs) const -> result_type
	{
		return (lhs - rhs);
	}
};

template<typename LHSType, typename RHSType>
struct Multiplies
{
	using first_argument_type = LHSType;
	using second_argument_type = RHSType;
	using result_type = decltype(std::declval<LHSType>() * std::declval<RHSType>());

	constexpr auto operator()(const LHSType& lhs, const RHSType& rhs) const -> result_type
	{
		return (lhs * rhs);
	}
};

template<typename Type>
struct Multiplies<Type, Type>
{
	using first_argument_type = Type;
	using second_argument_type = Type;
	using result_type = decltype(std::declval<Type>() * std::declval<Type>());

	constexpr auto operator()(const Type& lhs, const Type& rhs) const -> result_type
	{
		return (lhs * rhs);
	}
};

template<typename LHSType, typename RHSType>
struct Divides
{
	using first_argument_type = LHSType;
	using second_argument_type = RHSType;
	using result_type = decltype(std::declval<LHSType>() / std::declval<RHSType>());

	constexpr auto operator()(const LHSType& lhs, const RHSType& rhs) const -> result_type
	{
		return (lhs / rhs);
	}
};

template<typename Type>
struct Divides<Type, Type>
{
	using first_argument_type = Type;
	using second_argument_type = Type;
	using result_type = decltype(std::declval<Type>() / std::declval<Type>());

	constexpr auto operator()(const Type& lhs, const Type& rhs) const -> result_type
	{
		return (lhs / rhs);
	}
};

template<typename LHSType, typename RHSType>
struct Modulus
{
	using first_argument_type = LHSType;
	using second_argument_type = RHSType;
	using result_type = decltype(std::declval<LHSType>() % std::declval<RHSType>());

	constexpr auto operator()(const LHSType& lhs, const RHSType& rhs) const -> result_type
	{
		return (lhs % rhs);
	}
};

template<typename Type>
struct Modulus<Type, Type>
{
	using first_argument_type = Type;
	using second_argument_type = Type;
	using result_type = decltype(std::declval<Type>() % std::declval<Type>());

	constexpr auto operator()(const Type& lhs, const Type& rhs) const -> result_type
	{
		return (lhs % rhs);
	}
};

} // namespace Cry::Functional
