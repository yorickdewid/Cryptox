// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/ReferenceValue.h>

namespace CryCC
{
namespace SubValue
{
namespace Valuedef
{

//using namespace CryCC::SubValue::Typedef;

ReferenceValue::ReferenceValue(Value2&& value)
    : m_refValue{ std::make_unique<Value2>(std::move(value)) }
{
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
	//
}

// Convert data stream into reference value.
void ReferenceValue::Deserialize(ReferenceValue&, buffer_type&)
{
	//
}

// Compare to other ReferenceValue.
bool ReferenceValue::operator==(const ReferenceValue& other) const
{
    return m_refValue == other.m_refValue;
}

// Convert current value to string.
std::string ReferenceValue::ToString() const
{
	return "(ref)"; //TODO
}

} // namespace Valuedef
} // namespace SubValue
} // namespace CryCC
