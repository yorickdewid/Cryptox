// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/SubValue/ValueContract.h>
#include <CryCC/SubValue/PrimitiveTypes.h>
#include <CryCC/SubValue/Typedef.h>

#include <Cry/Cry.h>
#include <Cry/TypeTrait.h>

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
	//TODO: missing UnsignedLongDoubleType ?
	using NativeTypeList = Cry::TypeTrait::TemplateHolder<Typedef::CharType::storage_type
		, Typedef::ShortType::storage_type
		, Typedef::IntegerType::storage_type
		, Typedef::LongType::storage_type
		, Typedef::UnsignedCharType::storage_type
		, Typedef::UnsignedShortType::storage_type
		, Typedef::UnsignedIntegerType::storage_type
		, Typedef::UnsignedLongType::storage_type
		, Typedef::FloatType::storage_type
		, Typedef::DoubleType::storage_type
		, Typedef::LongDoubleType::storage_type>;
	using ValueVariant = NativeTypeList::template_apply<boost::variant>;

	ValueVariant m_value;

	struct PackerVisitor;

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
	template<typename Type, typename = typename std::enable_if<NativeTypeList::has_type<Type>::value>::type>
	BuiltinValue(Type&&);

	template<>
	BuiltinValue(Typedef::IntegerType::alias&& value)
		: m_value{ std::forward<Typedef::IntegerType::storage_type>(value) }
	{
	}

	template<>
	BuiltinValue(Typedef::UnsignedIntegerType::alias&& value)
		: m_value{ std::forward<Typedef::UnsignedIntegerType::storage_type>(value) }
	{
	}

	template<>
	BuiltinValue(Typedef::FloatType::alias&& value)
		: m_value{ std::forward<Typedef::FloatType::storage_type>(value) }
	{
	}

	template<>
	BuiltinValue(Typedef::DoubleType::alias&& value)
		: m_value{ std::forward<Typedef::DoubleType::storage_type>(value) }
	{
	}

	template<typename ReturnType>
	ReturnType As()
	{
		try {
			return boost::strict_get<ReturnType>(m_value);
		}
		catch (const boost::bad_get&) {
			throw InvalidTypeCastException{};
		}
	}

	//
	// Implement value category contract.
	//

	// Convert single value into data stream.
	static void Serialize(const BuiltinValue&, buffer_type&);
	// Convert data stream into single value.
	static void Deserialize(BuiltinValue&, buffer_type&);

	// Compare to other BuiltinValue.
	bool operator==(const BuiltinValue&) const;

	// Convert current value to string.
	std::string ToString() const;
};

static_assert(std::is_copy_constructible<BuiltinValue>::value, "BuiltinValue !is_copy_constructible");
static_assert(std::is_move_constructible<BuiltinValue>::value, "BuiltinValue !is_move_constructible");
static_assert(std::is_copy_assignable<BuiltinValue>::value, "BuiltinValue !is_copy_assignable");
static_assert(std::is_move_assignable<BuiltinValue>::value, "BuiltinValue !is_move_assignable");

} // namespace Valuedef
} // namespace SubValue
} // namespace CryCC
