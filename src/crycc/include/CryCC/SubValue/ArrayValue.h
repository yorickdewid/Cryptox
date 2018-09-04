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
#include <Cry/Algorithm.h>

#include <boost/variant.hpp>

#include <cstdint>

namespace CryCC::SubValue::Valuedef
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

	void ConstructFromType();

public:
	using typdef_type = Typedef::ArrayType;
	using value_category = ValueCategory::Plural;

	// Expose the value variants that this category can process.
	inline constexpr static const int value_variant_order = ArrayTypeList::size;
	// Unique value identifier.
	inline constexpr static const int value_category_identifier = 11;

	ArrayValue(const ArrayValue&) = default;
	ArrayValue(ArrayValue&&) = default;

	ArrayValue& operator=(const ArrayValue&) = default;
	ArrayValue& operator=(ArrayValue&&) = default;

	// Initialize the type variant with a primitive type.
	template<typename Type, typename = typename std::enable_if<
		!std::is_same<Type, std::add_lvalue_reference<ArrayValue>::type>::value
		&& !std::is_same<Type, ArrayValue>::value
	>::type>
		ArrayValue(std::initializer_list<Type>&& value)
		: m_value{ std::vector<Typedef::PrimitiveSelectorStorageType<Type>>{ value.begin(), value.end() } }
	{
		static_assert(ArrayTypeList::has_type<std::vector<Typedef::PrimitiveSelectorStorageType<Type>>>::value);
	}

	template<>
	ArrayValue(std::initializer_list<Value2>&& valueList)
		: m_value{ std::move(valueList) }
	{
	}

	virtual void ValueInit() override
	{
		ConstructFromType();
	}

	virtual ~ArrayValue() {}

	//
	// Implement multiordinal contract.
	//

	template<typename ReturnType>
	auto As() const -> std::vector<ReturnType>
	{
		try {
			auto valueList = boost::strict_get<std::vector<Typedef::PrimitiveSelectorStorageType<ReturnType>>>(m_value);
			std::vector<ReturnType> tmpVal;
			tmpVal.reserve(valueList.size());
			Cry::Algorithm::ContainerCast<ReturnType>(valueList.begin(), valueList.end(), std::back_inserter(tmpVal));
			return tmpVal;
		}
		catch (const boost::bad_get&) {
			throw InvalidTypeCastException{};
		}
	}

	//
	// Implement iterable contract.
	//

	size_type Size() const
	{
		return m_linkType->DataType<typdef_type>()->Order();
	}

	// Get the value at offset.
	template<typename ReturnType>
	auto At(offset_type offset) const
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

	// Emplace value at offset.
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

	//
	// Arithmetic operators.
	//

	ArrayValue& operator++() { throw InvalidValueArithmeticException{}; }
	ArrayValue& operator--() { throw InvalidValueArithmeticException{}; }
	ArrayValue operator++(int) { throw InvalidValueArithmeticException{}; }
	ArrayValue operator--(int) { throw InvalidValueArithmeticException{}; }

	friend ArrayValue operator+(const ArrayValue&, const ArrayValue&) { throw InvalidValueArithmeticException{}; }
	friend ArrayValue operator-(const ArrayValue&, const ArrayValue&) { throw InvalidValueArithmeticException{}; }
	friend ArrayValue operator*(const ArrayValue&, const ArrayValue&) { throw InvalidValueArithmeticException{}; }
	friend ArrayValue operator/(const ArrayValue&, const ArrayValue&) { throw InvalidValueArithmeticException{}; }
	friend ArrayValue operator%(const ArrayValue&, const ArrayValue&) { throw InvalidValueArithmeticException{}; }
};

static_assert(std::is_copy_constructible<ArrayValue>::value, "ArrayValue !is_copy_constructible");
static_assert(std::is_move_constructible<ArrayValue>::value, "ArrayValue !is_move_constructible");
static_assert(std::is_copy_assignable<ArrayValue>::value, "ArrayValue !is_copy_assignable");
static_assert(std::is_move_assignable<ArrayValue>::value, "ArrayValue !is_move_assignable");

} // namespace CryCC::SubValue::Valuedef
