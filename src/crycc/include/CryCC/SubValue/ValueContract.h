// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

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

// Abstract value is a value category helper.
template<typename ValueType>
struct AbstractValue : public ValueContract
{
    // Type alias to value category type.
    using self_type = ValueType;
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
	static auto Test(LHSType*) -> decltype(std::declval<LHSType>() == std::declval<RHSType>());
	template<typename, typename>
	static auto Test(...) -> std::false_type;

public:
	using type = typename std::is_same<bool, decltype(Test<Type, ArgType>(int{}))>::type;
};

template<class Type, class ArgType = Type>
struct HasEqualOperator : HasEqualOperatorImpl<Type, ArgType>::type {};

} // namespace Trait

template<typename Type>
struct IsValueContractCompliable
{
    constexpr static const bool value = std::is_base_of<AbstractValue<Type>, Type>::value
        && std::is_constructible<Type>::value
        && std::is_copy_constructible<Type>::value
        && std::is_move_constructible<Type>::value
        && std::is_copy_assignable<Type>::value
        && std::is_move_assignable<Type>::value
        && Trait::HasToString<Type>::value
        && Trait::HasEqualOperator<Type>::value;
};

} // namespace Valuedef
} // namespace SubValue
} // namespace CryCC
