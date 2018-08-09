// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Cry.h"

namespace Cry
{
namespace Algorithm
{

// The match-if algorithm will call the predicate for each element in the container
// and when evaluated to true the callback routine is called. This combines a
// predicate check and callback in one which is also possible with individual
// std algorithms.
template<typename InputIt, typename UnaryPredicate, typename UnaryCallback>
void MatchIf(InputIt first, InputIt last, UnaryPredicate predicate, UnaryCallback callback)
{
	for (; first != last; ++first) {
		if (predicate(*first)) {
			callback(first);
		}
	}
}

// Functor to match value with template parameter.
template<typename Type, Type Value>
struct MatchStatic
{
	using value_type = Type;

	constexpr bool operator()(const Type& Match) const
	{
		return (Match == Value);
	}
};

// Functor to match internal value.
template<typename Type>
struct MatchOn
{
	using value_type = Type;

	const Type value;

	constexpr MatchOn(Type val)
		: value{ val }
	{
	}
	
	constexpr bool operator()(const Type& val) const
	{
		return (val == value);
	}
};

} // namespace Algorithm
} // namespace Cry
