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

VariantType::VariantType(size_type elements, std::vector<BaseType> variantType)
    : m_elements{ elements }
    , m_elementTypes{ variantType }
{
    assert(elements == m_elementTypes.size()); //TODO also in release
}

VariantType::VariantType(size_type elements, std::vector<BaseType>&& variantType)
    : m_elements{ elements }
    , m_elementTypes{ std::move(variantType) }
{
    assert(elements == m_elementTypes.size()); //TODO also in release
}

const std::string VariantType::ToString() const
{
    return "(" + std::to_string(m_elements) + ")";
}

VariantType::size_type VariantType::UnboxedSize() const
{
    VariantType::size_type size = 0;
    for (const auto& type : m_elementTypes) {
        size += type->UnboxedSize();
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
