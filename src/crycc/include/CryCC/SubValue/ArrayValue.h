// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/SubValue/ValueContract.h>
#include <CryCC/SubValue/ArrayType.h>
#include <CryCC/SubValue/Valuedef.h>

#include <Cry/Cry.h>
#include <Cry/TypeTrait.h>
#include <Cry/Serialize.h>

#include <boost/variant.hpp>

#include <cstdint>

namespace CryCC
{
namespace SubValue
{
namespace Valuedef
{

class ArrayValue : public AbstractValue<ArrayValue>
{
	using ArrayTypeList = Cry::TypeTrait::TemplateHolder<std::vector<int8_t>
		, std::vector<int16_t>
		, std::vector<int32_t>
		, std::vector<int64_t>
		, std::vector<uint8_t>
		, std::vector<uint16_t>
		, std::vector<uint32_t>
		, std::vector<uint64_t>
		, std::vector<float>
		, std::vector<double>
		, std::vector<long double>
		, std::vector<Value2>>;
	using ValueVariant = ArrayTypeList::template_apply<boost::variant>;

	ValueVariant m_value;

public:
	using typdef_type = Typedef::ArrayType;
	using value_category = ValueCategory::Plural;

	// Expose the value variants that this category can process.
	constexpr static const int value_variant_order = ArrayTypeList::size;
	// Unique value identifier.
	constexpr static const int value_category_identifier = 11;

	ArrayValue() = default; //TODO: remove?
	ArrayValue(const ArrayValue&) = default;
	ArrayValue(ArrayValue&&) = default;

	ArrayValue& operator=(const ArrayValue&) = default;
	ArrayValue& operator=(ArrayValue&&) = default;

	template<typename ValueType, typename = typename std::enable_if<std::is_fundamental<ValueType>::value>::type> // TODO: check with NativeTypeList
	ArrayValue(std::initializer_list<ValueType>&&);

	template<>
	ArrayValue(std::initializer_list<int>&& valueList)
		: m_value{ std::move(valueList) }
	{
	}

	template<>
	ArrayValue(std::initializer_list<float>&& valueList)
		: m_value{ std::move(valueList) }
	{
	}

	template<>
	ArrayValue(std::initializer_list<double>&& valueList)
		: m_value{ std::move(valueList) }
	{
	}

	template<typename ReturnType>
	ReturnType As()
	{
		try {
			return boost::get<ReturnType>(m_value);
		}
		catch (const boost::bad_get&) {
			throw InvalidTypeCastException{};
		}
	}

	// Convert single value into data stream.
	static void Serialize(const ArrayValue&, Cry::ByteArray&);
	// Convert data stream into single value.
	static void Deserialize(ArrayValue&, Cry::ByteArray&);

	// Compare to other ArrayValue.
	bool operator==(const ArrayValue&) const;

	// Convert current value to string.
	std::string ToString() const;
};

} // namespace Valuedef
} // namespace SubValue
} // namespace CryCC
