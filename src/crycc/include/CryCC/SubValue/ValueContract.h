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

namespace CryCC
{
namespace SubValue
{
namespace Valuedef
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

};

// Abstract value is a value category helper.
template<typename ValueType>
struct AbstractValue : public ValueContract
{
	using self_type = ValueType;
	using size_type = size_t;
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

namespace Trait
{

template<typename Type>
using HasToString = std::is_same<decltype(std::declval<Type>().ToString()), std::string>;

template<class Type, class ArgType>
class HasEqualOperatorImpl
{
	template<class LHSType, class RHSType>
	constexpr static auto Test(int) -> decltype(std::declval<LHSType>() == std::declval<RHSType>());
	template<typename, typename>
	constexpr static auto Test(...) -> std::false_type;

public:
	using type = typename std::is_same<bool, decltype(Test<Type, ArgType>(int{}))>::type;
};

template<class Type, class ArgType = Type>
struct HasEqualOperator : HasEqualOperatorImpl<Type, ArgType>::type {};

template<class Type>
class HasTypedefTypeImpl
{
	template <typename ValueType>
	constexpr static auto Test(int) -> typename ValueType::typdef_type;
	template<typename>
	constexpr static auto Test(...) -> std::false_type;

public:
	constexpr static const bool value = std::is_same<std::false_type, decltype(Test<Type>(int{}))>::value;
	using type = typename std::bool_constant<!bool(value)>::type;
};

template<class Type>
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
//   6.) Have a string cast method, ofter 'ToString'.
//   7.) A type definition pointing to the type system.
//   8.) A value category.
//   9.) A variant order to communicate the number of variant values.
//  10.) An unique value category identifier.
//
// Within the contract set confinements a value is itself responsible for
// the processing, storage and structure of the value.
template<typename Type>
struct IsValueContractCompliable
{
	constexpr static const bool value = std::is_base_of<AbstractValue<Type>, Type>::value
		&& std::is_copy_constructible<Type>::value
		&& std::is_move_constructible<Type>::value
		&& std::is_copy_assignable<Type>::value
		&& std::is_move_assignable<Type>::value
		&& Trait::HasToString<Type>::value
		&& Trait::HasEqualOperator<Type>::value
        && Trait::HasTypedefType<Type>::value;
};

template<typename Type>
struct IsValueIterable
{
	constexpr static const bool value = std::is_base_of<IterableContract, Type>::value;
};

} // namespace Valuedef
} // namespace SubValue
} // namespace CryCC
