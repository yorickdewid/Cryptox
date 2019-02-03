// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/RecordType.h>

#include <numeric>

namespace CryCC::SubValue::Typedef
{

RecordType::buffer_type& operator<<(RecordType::buffer_type& os, const RecordType::Specifier& specifier)
{
	os << static_cast<Cry::Byte>(specifier);
	return os;
}

RecordType::buffer_type& operator>>(RecordType::buffer_type& os, RecordType::Specifier& specifier)
{
	specifier = RecordType::Specifier::STRUCT;
	os >> reinterpret_cast<Cry::Byte&>(specifier);
	return os;
}

RecordType::RecordType(Specifier specifier)
	: m_specifier{ specifier }
{
}

RecordType::RecordType(const std::string& name, Specifier specifier)
	: m_name{ name }
	, m_specifier{ specifier }
{
}

RecordType::RecordType(buffer_type& buffer)
{
	Unpack(buffer);
}

void RecordType::Pack(buffer_type& buffer) const
{
	AbstractType::Pack(buffer);

	buffer << m_aligned;
	buffer << m_name;
	buffer << m_specifier;
	buffer << m_fields.size();

	for (const auto& field : m_fields) {
		buffer << field.Name();
		field.Type()->Pack(buffer);
	}
}

void RecordType::Unpack(buffer_type& buffer)
{
	AbstractType::Unpack(buffer);

	buffer >> m_aligned;
	buffer >> m_name;
	buffer >> m_specifier;

	size_t elementCount{ 0 };
	buffer >> elementCount;
	for (size_t i = 0; i < elementCount; ++i) {
		std::string fieldName;
		buffer >> fieldName;
		m_fields.emplace_back(std::move(fieldName), std::move(TypeCategoryDeserialise(buffer)));
	}
}

void RecordType::AddField(const std::string& field, const InternalBaseType& type)
{
	m_fields.push_back(FieldType{ field, type });
}

void RecordType::AddField(std::string&& field, InternalBaseType&& type)
{
	m_fields.emplace_back(std::move(field), std::move(type));
}

void RecordType::AddField(FieldType&& field)
{
	m_fields.emplace_back(std::move(field));
}

const std::string RecordType::ToString() const
{
	std::string specifierStr;
	switch (m_specifier)
	{
	case Specifier::STRUCT:
		specifierStr = "struct";
		break;
	case Specifier::UNION:
		specifierStr = "union";
		break;
	case Specifier::CLASS:
		specifierStr = "class";
		break;
	default:
		break;
	}

	return specifierStr + " " + m_name;
}

RecordType::size_type RecordType::UnboxedSize() const
{
	return std::accumulate(m_fields.cbegin(), m_fields.cend(), RecordType::size_type{ 0 },
		[](RecordType::size_type total, const auto& field)
	{
		return std::move(total) + field.Type()->UnboxedSize();
	});
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
