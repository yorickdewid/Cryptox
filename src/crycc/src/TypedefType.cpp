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

TypedefType::TypedefType(const std::string& name, BaseType& nativeType)
	: m_name{ name }
	, m_resolveType{ nativeType }
{
}

TypedefType::TypedefType(const std::string& name, BaseType&& nativeType)
	: m_name{ name }
	, m_resolveType{ std::move(nativeType) }
{
}

const std::string TypedefType::TypeName() const
{
    return m_name + ":" + m_resolveType->TypeName();
}

TypedefType::size_type TypedefType::UnboxedSize() const
{
    return m_resolveType->UnboxedSize();
}

bool TypedefType::Equals(BasePointer other) const
{
	auto self = dynamic_cast<TypedefType*>(other);
	if (self == nullptr) {
		return false;
	}

	return m_resolveType == self->m_resolveType
		&& m_name == self->m_name;
}

TypedefType::buffer_type TypedefType::TypeEnvelope() const
{
	std::vector<uint8_t> buffer = { m_c_internalType };
	buffer.reserve(m_name.size());
	buffer.push_back(static_cast<uint8_t>(m_name.size())); //FUTURE: Limited to 256
	buffer.insert(buffer.cend(), m_name.cbegin(), m_name.cend());

	if (m_resolveType) {
		auto envelop = m_resolveType->TypeEnvelope();
		buffer.insert(buffer.cend(), envelop.cbegin(), envelop.cend());
	}

	const auto base = TypedefBase::TypeEnvelope();
	buffer.insert(buffer.cend(), base.cbegin(), base.cend());
	return buffer;
}

} // namespace CryCC::SubValue::Typedef
