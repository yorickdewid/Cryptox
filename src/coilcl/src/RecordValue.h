// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/Cry.h>
#include <Cry/Serialize.h>

#include <vector>
#include <memory>
#include <ostream>
#include <string>

//FUTURE:
// - Field class

namespace CoilCl
{
namespace Valuedef
{

class Value;

// Pointer to the value since value is not yet defined. The fields are kept in
// a vector since the order of fields is important.
class RecordValue
{
	std::string m_name;
	std::vector<std::pair<std::string, std::shared_ptr<Value>>> m_fields;

	bool Compare(const RecordValue&) const;

public:
	struct FieldExistException : public std::exception
	{
		const std::string m_field;
		FieldExistException(const std::string& field)
			: m_field{ field }
		{
		}

		inline std::string Field() const noexcept { return m_field; }
	};

public:
	RecordValue() = default;
	RecordValue(const std::string& name)
		: m_name{ name }
	{
	}

	// Add field to record
	void AddField(std::pair<std::string, std::shared_ptr<Value>>&& val)
	{
		if (HasField(val.first)) {
			throw FieldExistException{ val.first };
		}
		m_fields.push_back(std::move(val));
	}

	// Add field to record directly
	template<typename... ArgsType>
	void EmplaceField(ArgsType&&... args)
	{
		m_fields.emplace_back(std::forward<ArgsType>(args)...);
	}

	// Check if record has name.
	bool HasRecordName() const noexcept { return !m_name.empty(); }
	// Get record name.
	std::string RecordName() const noexcept { return m_name; }
	// Return number of fields.
	size_t Size() const noexcept { return m_fields.size(); }
	// Get the fieldname by index.
	const std::string FieldName(size_t idx) const { return m_fields.at(idx).first; }
	// Get the value by index.
	std::shared_ptr<Value> At(size_t idx) const { return m_fields.at(idx).second; }
	// Get the value by index.
	std::shared_ptr<Value> operator[](size_t idx) const { return m_fields.at(idx).second; }

	// Check if field with name already exists in this record.
	bool HasField(const std::string&) const;
	// Get the value by field name.
	std::shared_ptr<Value> GetField(const std::string&) const;

	// Convert record value into data stream.
	static void Serialize(const RecordValue&, Cry::ByteArray&);
	// Convert data stream into record value.
	static void Deserialize(RecordValue&, Cry::ByteArray&);

	bool operator==(const RecordValue& other) const
	{
		return Compare(other);
	}

	friend std::ostream& operator<<(std::ostream& os, const RecordValue& other)
	{
		os << (other.HasRecordName() ? other.m_name : "<anonymous record>");
		return os;
	}

	std::string ToString() const
	{
		return HasRecordName() ? m_name : "<anonymous record>";
	}

	template<typename Type>
	inline static auto AutoValue(Type val) -> std::shared_ptr<Type>
	{
		return std::make_shared<Type>(val);
	}
};

static_assert(std::is_copy_constructible<RecordValue>::value, "RecordValue !is_copy_constructible");
static_assert(std::is_move_constructible<RecordValue>::value, "RecordValue !is_move_constructible");
static_assert(std::is_copy_assignable<RecordValue>::value, "RecordValue !is_copy_assignable");
static_assert(std::is_move_assignable<RecordValue>::value, "RecordValue !is_move_assignable");

} // namespace Valuedef
} // namespace CoilCl
