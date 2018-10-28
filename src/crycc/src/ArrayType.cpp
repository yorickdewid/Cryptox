// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/ArrayType.h>

#include <Cry/Serialize.h>

#include <string>

namespace CryCC::SubValue::Typedef
{

ArrayType::ArrayType(size_t elements, InternalBaseType& arrayType)
	: m_elements{ elements }
	, m_elementType{ arrayType }
{
}

ArrayType::ArrayType(size_t elements, InternalBaseType&& arrayType)
	: m_elements{ elements }
	, m_elementType{ std::move(arrayType) }
{
}

void ArrayType::Serialize(const ArrayType& type, buffer_type& buffer)
{
	AbstractType::Serialize(dynamic_cast<const AbstractType&>(type), buffer);

	//TODO:
}

void ArrayType::Deserialize(ArrayType& type, buffer_type& buffer)
{
	AbstractType::Deserialize(dynamic_cast<AbstractType&>(type), buffer);

	//TODO:
}

const std::string ArrayType::ToString() const
{
	return "[" + std::to_string(m_elements) + "]";
}

ArrayType::size_type ArrayType::UnboxedSize() const
{
	return m_elements * m_elementType->UnboxedSize();
}

bool ArrayType::Equals(InternalBaseType* /*other*/) const
{
	/*auto self = dynamic_cast<ArrayType*>(other);
	if (self == nullptr) {
		return false;
	}

	return true;*/
	return false;
}

} // namespace CryCC::SubValue::Typedef
