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

namespace CryCC
{
namespace SubValue
{
namespace Valuedef
{

class BuiltinValue : public AbstractValue<BuiltinValue>
{
    using NativeTypeList = Cry::TypeTrait::TemplateHolder<int, char, float, double, bool>;
    using ValueVariant = NativeTypeList::template_apply<boost::variant>;

    ValueVariant m_value;

public:
    using typdef_type = Typedef::BuiltinType;
    using value_category = ValueCategory::Plural;

    // Expose the value variants that this category can process.
    constexpr static const int value_variant_order = NativeTypeList::size;
    // Unique value identifier.
    constexpr static const int value_category_identifier = 10;

	BuiltinValue() = default;
    BuiltinValue(const BuiltinValue&) = default;
    BuiltinValue(BuiltinValue&&) = default;

    BuiltinValue& operator=(const BuiltinValue&) = default;
    BuiltinValue& operator=(BuiltinValue&&) = default;

    template<typename Type>
    BuiltinValue(Type&& value)
        : m_value{ std::forward<Type>(value) }
    {
    }

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
