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

class SingleValue : public AbstractValue<SingleValue>
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

	SingleValue() = default;
    SingleValue(const SingleValue&) = default;
    SingleValue(SingleValue&&) = default;

    SingleValue& operator=(const SingleValue&) = default;
    SingleValue& operator=(SingleValue&&) = default;

    template<typename Type>
    SingleValue(Type&& value)
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
	static void Serialize(const SingleValue&, Cry::ByteArray&)
    {

    }

	// Convert data stream into single value.
	static void Deserialize(SingleValue&, Cry::ByteArray&)
    {

    }

    // Compare to other SingleValue.
	bool operator==(const SingleValue&) const
	{
		return false;
	}

    // Convert current value to string.
	std::string ToString() const
	{
		return "REPLACE ME"; //TODO
	}
};

} // namespace Valuedef
} // namespace SubValue
} // namespace CryCC
