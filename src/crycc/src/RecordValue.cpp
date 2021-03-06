// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

// Project includes.
#include <CryCC/SubValue/RecordValue.h>
#include <CryCC/SubValue/Valuedef.h>
#include <CryCC/SubValue/ValueHelper.h>

// Language includes.
#include <algorithm>

// Magic value for serialization and deserialization.
inline constexpr static const Cry::Byte valueMagic{ 0x8d };

namespace CryCC::SubValue::Valuedef
{

//TODO:
bool RecordValue::Compare(const RecordValue& other) const
{
	CRY_UNUSED(other);
	return true;
	/*if (m_fields.size() != other.m_fields.size()) { return false; }
	if (m_fields.empty() || other.m_fields.empty()) {
		return m_fields.empty() == other.m_fields.empty();
	}

	return std::equal(m_fields.cbegin(), m_fields.cend()
		, other.m_fields.cbegin(), other.m_fields.cend()
		, [](decltype(m_fields)::value_type itFirst, decltype(other.m_fields)::value_type itEnd)
	{
		return itFirst.first == itEnd.first
			&& ((*itFirst.second) == (*itEnd.second));
	});*/
}

void RecordValue::ConstructFromType()
{
	//TODO:
	m_linkType->DataType<typdef_type>();
}

//void RecordValue::AddField(std::pair<std::string, std::shared_ptr<Value>>&& val)
//{
//	if (HasField(val.first)) {
//		throw FieldExistException{ val.first };
//	}
//	m_fields.push_back(std::move(val));
//}

void RecordValue::AddField(int index, value_Type&& val)
{
	m_fields2.emplace(index, std::move(val));
}

//bool RecordValue::HasField(const std::string& name) const
//{
//	return std::any_of(m_fields.cbegin(), m_fields.cend(), [=](const auto& pair)
//	{
//		return pair.first == name;
//	});
//}

//std::shared_ptr<Value> RecordValue::GetField(const std::string& name) const
//{
//	return std::find_if(m_fields.cbegin(), m_fields.cend(), [=](const auto& pair)
//	{
//		return pair.first == name;
//	})->second;
//}

// Get the value at offset.
RecordValue::value_Type RecordValue::At(offset_type offset) const
{
	if (m_fields2.size() < offset + 1) {
		throw OutOfBoundsException{};
	}
	return m_fields2.at(offset);
}

// Emplace value at offset.
void RecordValue::Emplace(offset_type offset, value_Type&& value)
{
	if (m_fields2.size() < offset + 1) {
		throw OutOfBoundsException{};
	}
	m_fields2.at(offset) = std::move(value);

	//TODO: trigger type update
}

//TODO:
// Convert record value into data stream.
void RecordValue::Serialize(const RecordValue& /*value*/, buffer_type& buffer)
{
	// Since the record value can be used without value the serialize method
	// functions in a different manner. If the buffer is empty write the magic
	// value and platform settings. Otherwise append to current buffer.
	if (buffer.empty()) {
		buffer.SetMagic(valueMagic);
		buffer.SetPlatformCompat();
	}

	// Write the fields
	//buffer.SerializeAs<Cry::Word>(value.m_fields.size());
	//for (const auto& field : value.m_fields) {
		// Field name
		//buffer.SerializeAs<Cry::Word>(field.first.size());
		//buffer.insert(buffer.cend(), field.first.cbegin(), field.first.cend());

		// Field value
		//Value2::Serialize((*field.second), buffer);
	//}
}

// Convert data stream into record value. The passed buffer must be a subbuffer
// initialized on zero or an existing buffer configured on an offset.
void RecordValue::Deserialize(RecordValue& value, buffer_type& buffer)
{
	if (buffer.ValidateMagic(valueMagic)) {
		if (!buffer.IsPlatformCompat()) {
			CryImplExcept(); //TODO
		}
	}
	else {
		--buffer;
	}

	// Create temporary record.
	RecordValue tempRecord;

	const size_t fieldSize = buffer.Deserialize<Cry::Word>(Cry::ByteArray::AUTO);
	for (size_t i = 0; i < fieldSize; ++i) {
		// Field name
		std::string fieldName;
		const size_t fieldNameSize = buffer.Deserialize<Cry::Word>(Cry::ByteArray::AUTO);
		std::copy(buffer.cbegin() + buffer.Offset(), buffer.cbegin() + buffer.Offset() + fieldNameSize, std::back_inserter(fieldName));
		buffer.SetOffset(static_cast<int>(fieldNameSize)); //TODO: Make ByteArray do this automatically

		//TODO:
		// Field value
		//Valuedef::Value tmp = Util::MakeInt(0); //TODO: make uninitialized value
		//Value::Deserialize(tmp, buffer);
		//tempRecord.AddField({ fieldName, RecordValue::AutoValue(tmp) });
	}

	std::swap(value, tempRecord);
}

// Compare to other RecordValue.
bool RecordValue::operator==(const RecordValue& other) const
{
	return Compare(other);
}

// Convert current value to string.
std::string RecordValue::ToString() const
{
	return "(record)";
}

} // namespace CryCC::SubValue::Valuedef
