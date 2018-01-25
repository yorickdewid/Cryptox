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

// The match if algorithm will call the predicate for each element in the container
// and when evaluated to true the callback routine is called. This combines a
// predicate check and callback in one which is also possible with individual
// std algorithms.
template<typename _InputIt, typename _UnaryPredicate, typename _UnaryCallback>
void match_if(_InputIt first, _InputIt last, _UnaryPredicate p, _UnaryCallback c)
{
	for (; first != last; ++first) {
		if (p(*first)) {
			c(first);
		}
	}
}

} // namespace Except
} // namespace Cry
