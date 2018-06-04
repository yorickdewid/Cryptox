// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <vector>
#include <memory>

namespace CoilCl
{
namespace Valuedef
{

class Value;

// Pointer to the value since value is not yet defined. The fields are kept in
// a vector since the order of fields is important.
class RecordValue
{
	const std::string m_name;
	std::vector<std::pair<std::string, std::shared_ptr<Value>>> m_fields;

public:
	struct FieldExistException : public std::exception
	{
		//
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
			throw FieldExistException{};
		}
		m_fields.push_back(std::move(val));
	}

	// Add field to record directly
	template<typename... ArgsType>
	void EmplaceField(ArgsType&&... args)
	{
		m_fields.emplace_back(std::forward<ArgsType>(args)...);
	}

	// Check if record has name
	bool HasRecordName() const noexcept { return !m_name.empty(); }
	// Get record name
	std::string RecordName() const noexcept { return m_name; }
	// Return number of fields
	size_t Size() const noexcept { return m_fields.size(); }
	// Get the value by index
	std::shared_ptr<Value> At(size_t idx) const { m_fields.at(idx).second; }
	// Get the value by index
	std::shared_ptr<Value> operator[](size_t idx) const { return m_fields.at(idx).second; }

	// Check if field with name already exists in this record
	bool HasField(const std::string& name) const
	{
		return std::any_of(m_fields.cbegin(), m_fields.cend(), [=](const decltype(m_fields)::value_type& pair)
		{
			return pair.first == name;
		});
	}

	bool operator==(const RecordValue& other) const
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
			return itFirst.first == itEnd.first;
			//&& itFirst.second.get() == itEnd.second.get();
		});
	}

	friend std::ostream& operator<<(std::ostream& os, const RecordValue& other)
	{
		os << other.HasRecordName() ? other.m_name : "<anonymous record>";
		return os;
	}

	template<typename Type>
	inline static auto Value(Type val) -> std::shared_ptr<Type>
	{
		return std::make_shared<Type>(val);
	}
};

} // namespace Valuedef
} // namespace CoilCl
