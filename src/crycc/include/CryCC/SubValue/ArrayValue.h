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
#include <CryCC/SubValue/PrimitiveTypes.h>
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

class ArrayValue : public AbstractValue<ArrayValue>, public IterableContract
{
	using ArrayTypeList = Cry::TypeTrait::TemplateHolder<std::vector<Typedef::CharType::storage_type>
		, std::vector<Typedef::ShortType::storage_type>
		, std::vector<Typedef::IntegerType::storage_type>
		, std::vector<Typedef::LongType::storage_type>
		, std::vector<Typedef::UnsignedCharType::storage_type>
		, std::vector<Typedef::UnsignedShortType::storage_type>
		, std::vector<Typedef::UnsignedIntegerType::storage_type>
		, std::vector<Typedef::UnsignedLongType::storage_type>
		, std::vector<Typedef::FloatType::storage_type>
		, std::vector<Typedef::DoubleType::storage_type>
		, std::vector<Typedef::LongDoubleType::storage_type>
		, std::vector<Value2>>;
	using ValueVariant = ArrayTypeList::template_apply<boost::variant>;

	ValueVariant m_value;

	struct PackerVisitor;

	friend struct ArrayAccess; //TODO: Still?

public:
	using typdef_type = Typedef::ArrayType;
	using value_category = ValueCategory::Plural;

	// Expose the value variants that this category can process.
	constexpr static const int value_variant_order = ArrayTypeList::size;
	// Unique value identifier.
	constexpr static const int value_category_identifier = 11;

	ArrayValue(const ArrayValue&) = default;
	ArrayValue(ArrayValue&&) = default;

	ArrayValue& operator=(const ArrayValue&) = default;
	ArrayValue& operator=(ArrayValue&&) = default;

	template<typename ValueType, typename = typename std::enable_if<ArrayTypeList::has_type<std::vector<ValueType>>::value>::type>
	ArrayValue(std::initializer_list<ValueType>&&);

	template<>
	ArrayValue(std::initializer_list<Typedef::IntegerType::alias>&& valueList)
		: m_value{ std::move(valueList) }
	{
	}

	template<>
	ArrayValue(std::initializer_list<Typedef::UnsignedIntegerType::alias>&& valueList)
		: m_value{ std::move(valueList) }
	{
	}

	template<>
	ArrayValue(std::initializer_list<Typedef::FloatType::alias>&& valueList)
		: m_value{ std::move(valueList) }
	{
	}

	template<>
	ArrayValue(std::initializer_list<Typedef::DoubleType::alias>&& valueList)
		: m_value{ std::move(valueList) }
	{
	}

	template<typename ReturnType>
	auto As() const -> typename std::add_const<ReturnType>::type
	{
		try {
			return boost::strict_get<ReturnType>(m_value);
		}
		catch (const boost::bad_get&) {
			throw InvalidTypeCastException{};
		}
	}

	//
	// Implement iterable contract.
	//

	template<typename ReturnType>
	auto At(offset_type offset) const -> typename std::add_const<ReturnType>::type
	{
		try {
			const auto& elementList = boost::strict_get<std::vector<ReturnType>>(m_value);
			if (elementList.size() < offset + 1) {
				throw OutOfBoundsException{};
			}
			return elementList.at(offset);
		}
		catch (const boost::bad_get&) {
			throw InvalidTypeCastException{};
		}
	}

	template<typename PrimitiveType>
	void Emplace(offset_type offset, PrimitiveType&& value)
	{
		try {
			auto& elementList = boost::strict_get<std::vector<PrimitiveType>>(m_value);
			if (elementList.size() < offset + 1) {
				throw OutOfBoundsException{};
			}
			elementList.emplace(elementList.begin() + offset, std::forward<PrimitiveType>(value));
		}
		catch (const boost::bad_get&) {
			throw InvalidTypeCastException{};
		}
	}

	//
	// Implement value category contract.
	//

	// Convert single value into data stream.
	static void Serialize(const ArrayValue&, buffer_type&);
	// Convert data stream into single value.
	static void Deserialize(ArrayValue&, buffer_type&);

	// Compare to other ArrayValue.
	bool operator==(const ArrayValue&) const;

	// Convert current value to string.
	std::string ToString() const;
};

static_assert(std::is_copy_constructible<ArrayValue>::value, "ArrayValue !is_copy_constructible");
static_assert(std::is_move_constructible<ArrayValue>::value, "ArrayValue !is_move_constructible");
static_assert(std::is_copy_assignable<ArrayValue>::value, "ArrayValue !is_copy_assignable");
static_assert(std::is_move_assignable<ArrayValue>::value, "ArrayValue !is_move_assignable");

} // namespace Valuedef
} // namespace SubValue
} // namespace CryCC
