// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/Cry.h>

namespace Cry
{
namespace Functional
{

template<typename ArgumentType>
struct AllTrue
{
	using argument_type = ArgumentType;
	using result_type = bool;

	constexpr bool operator()(const ArgumentType& _Left) const
	{
		CRY_UNUSED(_Left);
		return true;
	}
};

template<typename ArgumentType>
struct AllFalse
{
	using argument_type = ArgumentType;
	using result_type = bool;

	constexpr bool operator()(const ArgumentType& _Left) const
	{
		CRY_UNUSED(_Left);
		return false;
	}
};

template<typename Type>
struct IsStack : public std::false_type {};

template<typename Type, typename Alloc>
struct IsStack<std::stack<Type, Alloc>> : public std::true_type {};

template<typename Type>
struct Identity { using type = typename Type::value_type; };

} // namespace Functional
} // namespace Cry
