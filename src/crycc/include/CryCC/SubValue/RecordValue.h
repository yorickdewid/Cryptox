// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/SubValue/ValueContract.h>
#include <CryCC/SubValue/Typedef.h>
//#include <CryCC/SubValue/Typedef.h>

#include <Cry/Cry.h>

#include <vector>
#include <memory>
#include <ostream>
#include <string>

//FUTURE:
// - Field class

namespace CryCC
{
namespace SubValue
{
namespace Valuedef
{

class Value;

//FUTURE: initialize all fields in one go.
// Pointer to the value since value is not yet defined. The fields are kept in
// a vector since the order of fields is important.
class RecordValue : public AbstractValue<RecordValue>, public IterableContract
{
	std::vector<std::pair<std::string, std::shared_ptr<Value>>> m_fields; //TODO: only use offsets

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
	using typdef_type = Typedef::RecordType;
	using value_category = ValueCategory::Plural;

	// Expose the value variants that this category can process.
	constexpr static const int value_variant_order = 0;
	// Unique value identifier.
	constexpr static const int value_category_identifier = 13;

	// Add field to record.
	void AddField(std::pair<std::string, std::shared_ptr<Value>>&&); //TODO: replace with next line.
	//void AddField(const std::string&, Value2&&);

	// Add field to record directly. Although this method can benefit
	// from forwarding semantics it is recommended to use the 'AddField'
	// method when adding a field to the record.
	template<typename... ArgsType>
	void EmplaceField(ArgsType&&... args)
	{
		m_fields.emplace_back(std::forward<ArgsType>(args)...);
	}

	// Return number of fields.
	inline size_t Size() const noexcept { return m_fields.size(); } //TODO: nope, only in type
	// Get the fieldname by index.
	inline const std::string FieldName(size_t idx) const { return m_fields.at(idx).first; }
	// Get the value by index.
	inline std::shared_ptr<Value> At(size_t idx) const { return m_fields.at(idx).second; }
	// Get the value by index.
	inline std::shared_ptr<Value> operator[](size_t idx) const { return m_fields.at(idx).second; }
	
	// Check if field with name already exists in this record.
	bool HasField(const std::string&) const;
	// Get the value by field name.
	std::shared_ptr<Value> GetField(const std::string&) const;

	//
	// Implement value category contract.
	//

	// Convert record value into data stream.
	static void Serialize(const RecordValue&, buffer_type&);
	// Convert data stream into record value.
	static void Deserialize(RecordValue&, buffer_type&);

	// Compare to other RecordValue.
	bool operator==(const RecordValue& other) const;

	// Convert current value to string.
	std::string ToString() const;

	// Capture value and wrap inside a managed pointer.
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
} // namespace SubValue
} // namespace CryCC
