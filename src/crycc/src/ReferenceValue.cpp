// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/ReferenceValue.h>

namespace CryCC::SubValue::Valuedef
{

ReferenceValue::ReferenceValue(Value&& value)
    : m_refValue{ std::make_unique<Value>(std::move(value)) }
{
}

ReferenceValue::ReferenceValue(buffer_type&)
{
	// TODO:
}

ReferenceValue::ReferenceValue(const ReferenceValue& other)
{
	CRY_UNUSED(other);
}

ReferenceValue& ReferenceValue::operator=(const ReferenceValue& other)
{
	CRY_UNUSED(other);
	return (*this);
}

// Convert reference value into data stream.
void ReferenceValue::Serialize(const ReferenceValue&, buffer_type&)
{
	// TODO:
}

// Convert data stream into reference value.
void ReferenceValue::Deserialize(ReferenceValue&, buffer_type&)
{
	// TODO:
}

// Compare to other ReferenceValue.
bool ReferenceValue::operator==(const ReferenceValue& other) const
{
    return m_refValue == other.m_refValue;
}

// Convert current value to string.
std::string ReferenceValue::ToString() const
{
	return "(ref)"; //TODO:
}

ReferenceValue& ReferenceValue::operator++()
{
	// ++(*lhs.m_refValue);//TODO:
	return (*this);
}

ReferenceValue& ReferenceValue::operator--()
{
	// --(*lhs.m_refValue);//TODO:
	return (*this);
}

ReferenceValue ReferenceValue::operator++(int)
{
	ReferenceValue tmp = std::as_const(*this);
	// (*lhs.m_refValue)++;//TODO:
	return tmp;
}

ReferenceValue ReferenceValue::operator--(int)
{
	ReferenceValue tmp = std::as_const(*this);
	// (*lhs.m_refValue)--;//TODO:
	return tmp;
}

ReferenceValue operator+(const ReferenceValue& lhs, const ReferenceValue& rhs)
{
	return (*lhs.m_refValue) + (*rhs.m_refValue);
}

ReferenceValue operator-(const ReferenceValue& lhs, const ReferenceValue& rhs)
{
	return (*lhs.m_refValue) - (*rhs.m_refValue);
}

ReferenceValue operator*(const ReferenceValue& lhs, const ReferenceValue& rhs)
{
	return (*lhs.m_refValue) * (*rhs.m_refValue);
}

ReferenceValue operator/(const ReferenceValue& lhs, const ReferenceValue& rhs)
{
	return (*lhs.m_refValue) / (*rhs.m_refValue);
}

ReferenceValue operator%(const ReferenceValue& lhs, const ReferenceValue& rhs)
{
	return (*lhs.m_refValue) % (*rhs.m_refValue);
}

} // namespace CryCC::SubValue::Valuedef
