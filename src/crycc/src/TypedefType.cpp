// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/TypedefType.h>

namespace CryCC::SubValue::Typedef
{

TypedefType::TypedefType(const std::string& name, InternalBaseType& nativeType)
	: m_name{ name }
	, m_resolveType{ nativeType }
{
}

TypedefType::TypedefType(const std::string& name, InternalBaseType&& nativeType)
	: m_name{ name }
	, m_resolveType{ std::move(nativeType) }
{
}

TypedefType::TypedefType(buffer_type& buffer)
{
	Unpack(buffer);
}

void TypedefType::Pack(buffer_type& buffer) const
{
	AbstractType::Pack(buffer);

	buffer << m_name;
	m_resolveType->Pack(buffer);
}

void TypedefType::Unpack(buffer_type& buffer)
{
	AbstractType::Unpack(buffer);

	buffer >> m_name;
	m_resolveType = TypeCategoryDeserialise(buffer);
}

const std::string TypedefType::ToString() const
{
    return m_name + ":" + m_resolveType->ToString();
}

TypedefType::size_type TypedefType::UnboxedSize() const
{
    return m_resolveType->UnboxedSize();
}

bool TypedefType::Equals(InternalBaseType* /*other*/) const
{
	return false;
	/*auto self = dynamic_cast<TypedefType*>(other);
	if (self == nullptr) {
		return false;
	}

	return m_resolveType == self->m_resolveType
		&& m_name == self->m_name;*/
}

} // namespace CryCC::SubValue::Typedef
