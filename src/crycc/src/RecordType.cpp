// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/Typedef.h>

namespace CryCC
{
namespace SubValue
{
namespace Typedef
{

RecordType::RecordType(const std::string& name, Specifier specifier)
	: m_name{ name }
	, m_specifier{ specifier }
{
}

//RecordType::RecordType(const std::string& name, Specifier specifier, size_t elements, BaseType type);
//	: m_name{ name }
//	, m_specifier{ specifier }
//{
//}

bool RecordType::Equals(TypedefBase* other) const
{
	auto self = dynamic_cast<RecordType*>(other);
	if (self == nullptr) {
		return false;
	}

	return m_specifier == self->m_specifier
		&& m_name == self->m_name;
}

std::vector<uint8_t> RecordType::TypeEnvelope() const
{
	std::vector<uint8_t> buffer = { m_c_internalType };
	buffer.reserve(m_name.size());
	buffer.push_back(static_cast<uint8_t>(m_name.size())); //FUTURE: Limited to 256
	buffer.insert(buffer.cend(), m_name.cbegin(), m_name.cend());
	buffer.push_back(static_cast<uint8_t>(m_specifier));
	const auto base = TypedefBase::TypeEnvelope();
	buffer.insert(buffer.cend(), base.cbegin(), base.cend());
	return buffer;
}

} // namespace Typedef
} // namespace SubValue
} // namespace CryCC
