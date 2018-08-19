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
template<typename ValueType>
struct ValueContract
{
    // Type alias to value category type.
    using self_type = ValueType;

    // Get the type category as string.
    virtual std::string ToString() const = 0;
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
    constexpr static const bool value = std::is_base_of<ValueContract<Type>, Type>::value
        && std::is_constructible<SingleValue>::value
        && std::is_copy_constructible<SingleValue>::value
        && std::is_move_constructible<SingleValue>::value
        && std::is_copy_assignable<SingleValue>::value
        && std::is_move_assignable<SingleValue>::value
        && Trait::HasToString<Type>::value
        && Trait::HasEqualOperator<Type>::value;
};

} // namespace Valuedef
} // namespace SubValue
} // namespace CryCC
