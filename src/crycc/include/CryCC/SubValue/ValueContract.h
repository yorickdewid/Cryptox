// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/SubValue/TypeFacade.h>

#include <Cry/Cry.h>
#include <Cry/Serialize.h>

#include <string>

namespace CryCC::SubValue::Valuedef
{

struct ValueCategory
{
	struct Singular {};
	struct Plural {};
};

// The value contract must be implemented by classes that extend
// the value category system with a custom value implementation.
// The value contract is a pure virtual interface.
struct ValueContract
{
	// Get the type category as string.
	virtual std::string ToString() const = 0;
};

struct IterableContract
{
	using offset_type = size_t;
	using size_type = size_t;

	virtual size_type Size() const = 0;
};

// Abstract value is a value category helper.
template<typename ValueType>
struct AbstractValue : public ValueContract
{
	using self_type = ValueType;
	using buffer_type = Cry::ByteArray;

	// Value categories are encouraged to override this method and
	// cast the type pointer to the corresponding type.
	virtual void ReferenceType(const Typedef::TypeFacade *ptr)
	{
		m_linkType = ptr;
	}

	virtual void ValueInit() {}

protected:
	const Typedef::TypeFacade *m_linkType{ nullptr };
};

struct InvalidTypeCastException : public std::runtime_error
{
	explicit InvalidTypeCastException()
		: runtime_error{ "" } //TODO:
	{
	}
};

struct InvalidValueArithmeticException : public std::runtime_error
{
	explicit InvalidValueArithmeticException()
		: runtime_error{ "" } //TODO:
	{
	}
};

struct OutOfBoundsException : public std::runtime_error
{
	explicit OutOfBoundsException()
		: runtime_error{ "" } //TODO:
	{
	}
};

namespace Trait
{

template<typename Type>
using HasToString = std::is_same<decltype(std::declval<Type>().ToString()), std::string>;

template<typename Type>
struct HasArithmeticOps
{
	inline constexpr static const bool value = std::is_same<Type, decltype(std::declval<Type>() + std::declval<Type>())>::value
		&& std::is_same<Type, decltype(std::declval<Type>() - std::declval<Type>())>::value
		&& std::is_same<Type, decltype(std::declval<Type>() * std::declval<Type>())>::value
		&& std::is_same<Type, decltype(std::declval<Type>() / std::declval<Type>())>::value
		&& std::is_same<Type, decltype(std::declval<Type>() % std::declval<Type>())>::value;
};

template<typename Type>
inline constexpr bool HasArithmeticOps_v = HasArithmeticOps<Type>::value;

template<typename Type, typename ReturnType, typename... Args>
using TestSerializeMethod = std::is_same<decltype(std::declval<Type>().Serialize(std::declval<Args>()...)), ReturnType>;

template<typename Type>
using HasSerialize = TestSerializeMethod<Type, void
	, typename std::add_const<typename std::add_lvalue_reference<Type>::type>::type
	, typename std::add_lvalue_reference<typename Type::buffer_type>::type>;

template<typename Type, typename ReturnType, typename... Args>
using TestDeserializeMethod = std::is_same<decltype(std::declval<Type>().Deserialize(std::declval<Args>()...)), ReturnType>;

template<typename Type>
using HasDeserialize = TestDeserializeMethod<Type, void
	, typename std::add_lvalue_reference<Type>::type
	, typename std::add_lvalue_reference<typename Type::buffer_type>::type>;

template<typename Type, typename ArgType>
class HasEqualOperatorImpl
{
	template<typename LHSType, typename RHSType>
	constexpr static auto Test(int) -> decltype(std::declval<LHSType>() == std::declval<RHSType>());
	template<typename, typename>
	constexpr static auto Test(...) -> std::false_type;

public:
	using type = typename std::is_same<bool, decltype(Test<Type, ArgType>(int{}))>::type;
};

template<typename Type, typename ArgType = Type>
struct HasEqualOperator : HasEqualOperatorImpl<Type, ArgType>::type {};

template<typename Type>
class HasTypedefTypeImpl
{
	template <typename ValueType>
	constexpr static auto Test(int) -> typename ValueType::typdef_type;
	template<typename>
	constexpr static auto Test(...) -> std::false_type;

public:
	inline constexpr static const bool value = std::is_same<std::false_type, decltype(Test<Type>(int{}))>::value;
	using type = typename std::bool_constant<!bool{ value }>::type;
};

template<typename Type>
struct HasTypedefType : HasTypedefTypeImpl<Type>::type {};

} // namespace Trait

//TODO:
// - Test for value_category
// A value category must conform the value contract and additional
// clauses. Value categories must have at least:
//   1.) Be copy constructible.
//   2.) Be move constructible.
//   3.) Be copy assignable.
//   4.) Be move assignable.
//   5.) Be comparable to another instance of the same object.
//   6.) Have a string cast method, often 'ToString'.
//   7.) Have a static serialization method, often 'Serialize'.
//   8.) Have a static deserialization method, often 'Deserialize'.
//   9.) A type definition pointing to the type system.
//  10.) A value category.
//  11.) A variant order to communicate the number of variant values.
//  12.) An unique value category identifier.
//
// Within the contract set confinements a value is itself responsible for
// the processing, storage and structure of the value.
template<typename Type>
struct IsValueContractCompliable
{
	inline constexpr static const bool value = std::is_base_of<AbstractValue<Type>, Type>::value
		&& std::is_copy_constructible<Type>::value
		&& std::is_move_constructible<Type>::value
		&& std::is_copy_assignable<Type>::value
		&& std::is_move_assignable<Type>::value
		&& Trait::HasToString<Type>::value
		&& Trait::HasSerialize<Type>::value
		&& Trait::HasDeserialize<Type>::value
		&& Trait::HasEqualOperator<Type>::value
        && Trait::HasTypedefType<Type>::value
		&& Trait::HasArithmeticOps<Type>::value;
};

template<typename Type>
inline constexpr bool IsValueContractCompliable_v = IsValueContractCompliable<Type>::value;

template<typename Type>
struct IsValueMultiOrdinal
{
	inline constexpr static const bool value = std::bool_constant<bool{ Type::value_variant_order > 0 }>::value;
};

template<typename Type>
inline constexpr bool IsValueMultiOrdinal_v = IsValueMultiOrdinal<Type>::value;

template<typename Type>
struct IsValueIterable
{
	inline constexpr static const bool value = std::is_base_of<IterableContract, Type>::value;
};

template<typename Type>
inline constexpr bool IsValueIterable_v = IsValueIterable<Type>::value;

} // namespace CryCC::Valuedef::SubValue
