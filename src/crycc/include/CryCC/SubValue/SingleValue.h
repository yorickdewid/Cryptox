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

namespace CryCC
{
namespace SubValue
{
namespace Valuedef
{

// class Value;

class SingleValue : public ValueContract<SingleValue>
{
    using NativeTypeList = Cry::TypeTrait::TemplateHolder<int, char, float, double, bool>;
    // using ValueVariant = Cry::TypeTrait::TemplateHolder::template_apply<boost::variant>;

public:
    using typdef_type = Typedef::BuiltinType;
    using value_category = ValueCategory::Plural;

    // Expose the value variants that this category can process.
    constexpr static const int value_variant_order = NativeTypeList::size;
    // Unique value identifier.
    constexpr static const int value_category_identifier = 10;

	SingleValue() = default;

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
		return "";
	}
};

} // namespace Valuedef
} // namespace SubValue
} // namespace CryCC
