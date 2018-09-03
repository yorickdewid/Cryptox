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

namespace CryCC::SubValue::Valuedef
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
	inline constexpr static const int value_variant_order = NativeTypeList::size;
	// Unique value identifier.
	inline constexpr static const int value_category_identifier = 10;

	BuiltinValue(const BuiltinValue&) = default;
	BuiltinValue(BuiltinValue&&) = default;

	BuiltinValue& operator=(const BuiltinValue&) = default;
	BuiltinValue& operator=(BuiltinValue&&) = default;

	// Initialize the type variant with a primitive type.
	template<typename Type, typename = typename std::enable_if<
		!std::is_same<Type, std::add_lvalue_reference<BuiltinValue>::type>::value
	>::type>
	BuiltinValue(Type value)
	 	: m_value{ static_cast<Typedef::PrimitiveSelectorStorageType<Type>>(value) }
	{
		static_assert(NativeTypeList::has_type<Typedef::PrimitiveSelectorStorageType<Type>>::value);
	}

	//
	// Implement multiordinal contract.
	//

	template<typename ReturnType>
	auto As() const
	{
		try {
			return static_cast<ReturnType>(boost::strict_get<Typedef::PrimitiveSelectorStorageType<ReturnType>>(m_value));
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

} // namespace CryCC::SubValue::Valuedef
