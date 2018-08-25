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

class BuiltinValue : public AbstractValue<BuiltinValue>
{
	using NativeTypeList = Cry::TypeTrait::TemplateHolder<int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, float, double, long double>;
	using ValueVariant = NativeTypeList::template_apply<boost::variant>;

	ValueVariant m_value;

public:
	using typdef_type = Typedef::BuiltinType;
	using value_category = ValueCategory::Plural;

	// Expose the value variants that this category can process.
	constexpr static const int value_variant_order = NativeTypeList::size;
	// Unique value identifier.
	constexpr static const int value_category_identifier = 10;

	BuiltinValue(const BuiltinValue&) = default;
	BuiltinValue(BuiltinValue&&) = default;

	BuiltinValue& operator=(const BuiltinValue&) = default;
	BuiltinValue& operator=(BuiltinValue&&) = default;

	// Prevent template constructor from becoming copy/move constructor.
	template<typename Type, typename = typename std::enable_if<std::is_fundamental<Type>::value>::type> // TODO: check with NativeTypeList
	BuiltinValue(Type&&);

	//TODO: for now
	template<>
	BuiltinValue(int&& value)
		: m_value{ std::forward<int32_t>(value) }
	{
	}

	//TODO: for now
	template<>
	BuiltinValue(float&& value)
		: m_value{ std::forward<float>(value) }
	{
	}

	//TODO: for now
	template<>
	BuiltinValue(double&& value)
		: m_value{ std::forward<double>(value) }
	{
	}

	//TODO: redirect type aliasses to internal types like int8_t.

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
	static void Serialize(const BuiltinValue&, Cry::ByteArray&);
	// Convert data stream into single value.
	static void Deserialize(BuiltinValue&, Cry::ByteArray&);

	// Compare to other BuiltinValue.
	bool operator==(const BuiltinValue&) const;

	// Convert current value to string.
	std::string ToString() const;
};

} // namespace Valuedef
} // namespace SubValue
} // namespace CryCC
