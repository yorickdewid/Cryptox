// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/VariantType.h>

#include <cassert>
#include <string>

namespace CryCC::SubValue::Typedef
{

VariantType::VariantType(std::vector<BaseType>& variantType)
	: m_elementTypes{ variantType }
{
}

VariantType::VariantType(std::vector<BaseType>&& variantType)
	: m_elementTypes{ std::move(variantType) }
{
}

VariantType::VariantType(std::initializer_list<BaseType>&& variantType)
	: m_elementTypes{ std::move(variantType) }
{
}

VariantType::VariantType(buffer_type& buffer)
{
	Unpack(buffer);
}

void VariantType::Pack(buffer_type& buffer) const
{
	AbstractType::Pack(buffer);

	buffer << m_elementTypes.size();
	for (const auto& type : m_elementTypes) {
		type->Pack(buffer);
	}
}

void VariantType::Unpack(buffer_type& buffer)
{
	AbstractType::Unpack(buffer);

	size_t elementCount{ 0 };
	buffer >> elementCount;
	for (size_t i = 0; i < elementCount; ++i) {
		m_elementTypes.emplace_back(std::move(TypeCategoryDeserialise(buffer)));
	}
}

const std::string VariantType::ToString() const
{
	return "(" + std::to_string(m_elementTypes.size()) + ")";
}

VariantType::size_type VariantType::UnboxedSize() const
{
	VariantType::size_type size = 0;
	for (const auto& type : m_elementTypes) {
		size += type->UnboxedSize(); // TODO: sum?
	}
	return size;
}

//TODO:
bool VariantType::Equals(InternalBaseType* /*other*/) const
{
	/*auto self = dynamic_cast<VariantType*>(other);
	if (self == nullptr) {
		return false;
	}

	return true;*/
	return false;
}

} // namespace CryCC::SubValue::Typedef
