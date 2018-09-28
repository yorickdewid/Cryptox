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
#include <Cry/Types.h>
#include <Cry/TypeTrait.h>

#include <boost/variant.hpp>

#include <cstdint>

namespace CryCC::SubValue::Valuedef
{

class BuiltinValue : public AbstractValue<BuiltinValue>
{
	//TODO: missing UnsignedLongDoubleType, bool? ?
	using NativeTypeList = Cry::TypeTrait::TemplateHolder<Cry::CharType::storage_type
		, Cry::ShortType::storage_type
		, Cry::IntegerType::storage_type
		, Cry::LongType::storage_type
		, Cry::UnsignedCharType::storage_type
		, Cry::UnsignedShortType::storage_type
		, Cry::UnsignedIntegerType::storage_type
		, Cry::UnsignedLongType::storage_type
		, Cry::FloatType::storage_type
		, Cry::DoubleType::storage_type
		, Cry::LongDoubleType::storage_type>;
	using ValueVariant = NativeTypeList::template_apply<boost::variant>;

	ValueVariant m_value;

	struct PackerVisitor;

	template<typename Type>
	constexpr auto InitialConversion(Type value)
	{
		if constexpr (NativeTypeList::has_type<Type>::value) {
			return value;
		}
		else {
			static_assert(Cry::IsPrimitiveType_v<Type>,
				"cannot convert type to value category inner-presentation");
			return static_cast<Cry::PrimitiveSelectorStorageType<Type>>(value);
		}
	}

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
	template<typename Type, typename = typename std::enable_if_t<
		!std::is_same_v<Type, std::add_lvalue_reference<BuiltinValue>::type>
		&& !std::is_same_v<Type, BuiltinValue>
	>>
		BuiltinValue(Type value)
		: m_value{ InitialConversion(value) }
	{
	}

	//
	// Implement multiordinal contract.
	//

	template<typename ReturnType>
	auto As() const
	{
		try {
			return static_cast<ReturnType>(boost::strict_get<Cry::PrimitiveSelectorStorageType<ReturnType>>(m_value));
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

	//
	// Arithmetic operators.
	//

	BuiltinValue& operator++();
	BuiltinValue& operator--();
	BuiltinValue operator++(int);
	BuiltinValue operator--(int);

	friend BuiltinValue operator+(const BuiltinValue&, const BuiltinValue&);
	friend BuiltinValue operator-(const BuiltinValue&, const BuiltinValue&);
	friend BuiltinValue operator*(const BuiltinValue&, const BuiltinValue&);
	friend BuiltinValue operator/(const BuiltinValue&, const BuiltinValue&);
	friend BuiltinValue operator%(const BuiltinValue&, const BuiltinValue&);
};

static_assert(std::is_copy_constructible<BuiltinValue>::value, "BuiltinValue !is_copy_constructible");
static_assert(std::is_move_constructible<BuiltinValue>::value, "BuiltinValue !is_move_constructible");
static_assert(std::is_copy_assignable<BuiltinValue>::value, "BuiltinValue !is_copy_assignable");
static_assert(std::is_move_assignable<BuiltinValue>::value, "BuiltinValue !is_move_assignable");

} // namespace CryCC::SubValue::Valuedef
