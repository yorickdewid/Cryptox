// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/SubValue/ValueContract.h>
#include <CryCC/SubValue/Valuedef.h>

namespace CryCC::SubValue::Valuedef
{

#if 0

//TODO: make iterator
class ValueIterator
{
    size_t m_offset;
    IterableContract& m_iterValue;

public:
	// Initialize with iterator helper.
	ValueIterator(const typename Value2::iterator_helper_value_type& offsetValue)
        : m_iterValue{ offsetValue.m_iterValue }
        , m_offset{ offsetValue.Offset() }
	{
	}
	
    // Initialize with iterable value.
    template<typename ValueCategoryType, typename = typename std::enable_if<IsValueIterable<ValueCategoryType>::value>::type>
	ValueIterator(ValueCategoryType& value, size_t offset = 0)
		: m_iterValue{ value }
		, m_offset{ offset }
	{
	}

	void Get() {}

	void operator*() {}
};

#endif

} // namespace namespace CryCC::SubValue::Valuedef
