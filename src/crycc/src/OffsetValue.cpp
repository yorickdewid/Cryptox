// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/Valuedef.h>
#include <CryCC/SubValue/ArrayValue.h>
#include <CryCC/SubValue/OffsetValue.h>

namespace CryCC::SubValue::Valuedef
{

size_t SizeFromValue(const Value2& value)
{
	return value.ElementCount<ArrayValue>();
}

OffsetValue::OffsetValue(Value2& value, offset_type offset)
	: m_iterValue{ value }
	, m_offset{ offset }
{
	if (SizeFromValue(m_iterValue) <= m_offset) {
		CryImplExcept();
	}
}

OffsetValue::OffsetValue(const OffsetValue& other)
	: m_iterValue{ other.m_iterValue }
	, m_offset{ other.m_offset }
{}

OffsetValue& OffsetValue::operator=(const OffsetValue& other)
{
	m_iterValue = other.m_iterValue;
	m_offset = other.m_offset;
	return (*this);
}

OffsetValue& OffsetValue::operator++()
{
	if (SizeFromValue(m_iterValue) > m_offset) {
		++m_offset;
	}
	return (*this);
}

OffsetValue& OffsetValue::operator--()
{
	if (SizeFromValue(m_iterValue) > m_offset) {
		--m_offset;
	}
	return (*this);
}

OffsetValue OffsetValue::operator++(int)
{
	OffsetValue tmp = std::as_const(*this);
	if (SizeFromValue(m_iterValue) > m_offset) {
		m_offset++;
	}
	return tmp;
}

OffsetValue OffsetValue::operator--(int)
{
	OffsetValue tmp = std::as_const(*this);
	if (SizeFromValue(m_iterValue) > m_offset) {
		m_offset--;
	}
	return tmp;
}

} // namespace CryCC::SubValue::Valuedef
