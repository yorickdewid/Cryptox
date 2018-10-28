// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/RecordType.h>

namespace CryCC::SubValue::Typedef
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

const std::string RecordType::ToString() const
{
	return (m_specifier == Specifier::UNION ? "union " : "struct ") + m_name;
}

//TODO: calculate size
RecordType::size_type RecordType::UnboxedSize() const
{
	return 0;
}

bool RecordType::Equals(InternalBaseType* /*other*/) const
{
	return false;
	//auto self = dynamic_cast<RecordType*>(other);
	//if (self == nullptr) {
	//	return false;
	//}

	//return m_specifier == self->m_specifier
	//	&& m_name == self->m_name;
}

} // namespace CryCC::SubValue::Typedef
