// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/Typedef.h>
#include <CryCC/SubValue/ArrayType.h>

namespace CryCC
{
namespace SubValue
{
namespace Typedef
{

ArrayType::ArrayType()
{
}

const std::string ArrayType::TypeName() const
{
    return "";
}

bool ArrayType::Equals(TypedefBase* other) const
{
	auto self = dynamic_cast<RecordType*>(other);
	if (self == nullptr) {
		return false;
	}

    return true;
}

ArrayType::buffer_type ArrayType::TypeEnvelope() const
{
	// std::vector<uint8_t> buffer = { m_c_internalType };
	// buffer.reserve(m_name.size());
	// buffer.push_back(static_cast<uint8_t>(m_name.size())); //FUTURE: Limited to 256
	// buffer.insert(buffer.cend(), m_name.cbegin(), m_name.cend());
	// buffer.push_back(static_cast<uint8_t>(m_specifier));
	// const auto base = TypedefBase::TypeEnvelope();
	// buffer.insert(buffer.cend(), base.cbegin(), base.cend());
	// return buffer;
    return {};
}

} // namespace Typedef
} // namespace SubValue
} // namespace CryCC
