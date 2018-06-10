// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "RecordValue.h"
#include "Valuedef.h"

#include <algorithm>

using namespace CoilCl::Valuedef;

bool RecordValue::HasField(const std::string& name) const
{
	return std::any_of(m_fields.cbegin(), m_fields.cend(), [=](const decltype(m_fields)::value_type& pair)
	{
		return pair.first == name;
	});
}

bool RecordValue::Compare(const RecordValue& other) const
{
	if (m_name != other.m_name) { return false; }
	if (m_fields.size() != other.m_fields.size()) { return false; }
	if (m_fields.empty() || other.m_fields.empty()) {
		return m_fields.empty() == other.m_fields.empty();
	}

	return std::equal(m_fields.cbegin(), m_fields.cend()
		, other.m_fields.cbegin(), other.m_fields.cend()
		, [](decltype(m_fields)::value_type itFirst, decltype(other.m_fields)::value_type itEnd)
	{
		return itFirst.first == itEnd.first
			&& ((*itFirst.second) == (*itEnd.second));
	});
}

//// Copy assignable
//RecordValue& RecordValue::operator=(const RecordValue& other)
//{
//	m_name = other
//	return (*this);
//}
//
//// Move assignable
//RecordValue& RecordValue::operator=(RecordValue&& other)
//{
//	return (*this);
//}

#define VALUE_MAGIC 0x8d

// Convert record value into data stream.
void RecordValue::Serialize(const RecordValue& value, Cry::ByteArray& buffer)
{
	// Since the record value can be used without value the serialize method
	// functions in a different manner. If the buffer is empty write the magic
	// value and platform settings. Otherwise append to current buffer.
	if (buffer.empty()) {
		buffer.SetMagic(VALUE_MAGIC);
		buffer.SetPlatformCompat();
	}

	// Write record name even if empty
	buffer.SerializeAs<Cry::Word>(value.m_name.size());
	buffer.insert(buffer.cend(), value.m_name.cbegin(), value.m_name.cend());

	// Write the fields
	buffer.SerializeAs<Cry::Word>(value.m_fields.size());
	for (const auto& field : value.m_fields) {
		// Field name
		buffer.SerializeAs<Cry::Word>(field.first.size());
		buffer.insert(buffer.cend(), field.first.cbegin(), field.first.cend());
		// Field value
		//Value::Serialize((*field.second), buffer);
	}
	//std::vector<std::pair<std::string, std::shared_ptr<Value>>> m_fields;
}

// Convert data stream into record value. The passed buffer must be a subbuffer
// initialized on zero or an existing buffer configured on an offset.
void RecordValue::Deserialize(RecordValue& value, Cry::ByteArray& buffer)
{
	if (buffer.ValidateMagic(VALUE_MAGIC)) {
		if (!buffer.IsPlatformCompat()) {
			CryImplExcept(); //TODO
		}
	}
	
	// Read the record name
	std::string name;
	size_t nameSize = buffer.Deserialize<Cry::Word>(Cry::ByteArray::AUTO);
	if (nameSize) {
		std::copy(buffer.cbegin() + buffer.Offset(), buffer.cbegin() + buffer.Offset() + nameSize, std::back_inserter(name));
		buffer.StartOffset(buffer.Offset() + nameSize); //TODO: Make ByteArray do this automatically
	}

	// Create temporary record
	RecordValue tmpRec{ name };

	size_t fieldSize = buffer.Deserialize<Cry::Word>(Cry::ByteArray::AUTO);
	if (fieldSize) {
		// Field name
		std::string fieldName;
		size_t fieldNameSize = buffer.Deserialize<Cry::Word>(Cry::ByteArray::AUTO);
		std::copy(buffer.cbegin() + buffer.Offset(), buffer.cbegin() + buffer.Offset() + nameSize, std::back_inserter(fieldName));

		//tmpRec.AddField();
	}

	std::swap(value, RecordValue{ name });
}
