// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "RecordValue.h"
#include "Valuedef.h"

#include <algorithm>

using namespace CoilCl::Valuedef;

bool RecordValue::HasField(const std::string& name) const
{
	return std::any_of(m_fields.cbegin(), m_fields.cend(), [=](const decltype(m_fields)::value_type& pair)
	{
		return pair.first == name;
	});
}

bool RecordValue::Compare(const RecordValue& other) const
{
	if (m_name != other.m_name) { return false; }
	if (m_fields.size() != other.m_fields.size()) { return false; }
	if (m_fields.empty() || other.m_fields.empty()) {
		return m_fields.empty() == other.m_fields.empty();
	}

	return std::equal(m_fields.cbegin(), m_fields.cend()
		, other.m_fields.cbegin(), other.m_fields.cend()
		, [](decltype(m_fields)::value_type itFirst, decltype(other.m_fields)::value_type itEnd)
	{
		return itFirst.first == itEnd.first
			&& ((*itFirst.second) == (*itEnd.second));
	});
}
