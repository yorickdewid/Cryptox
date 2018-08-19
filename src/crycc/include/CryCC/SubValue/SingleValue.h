// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/SubValue/ValueContract.h>

#include <Cry/Cry.h>
#include <Cry/TypeTrait.h>
#include <Cry/Serialize.h>

#include <vector>
#include <memory>
#include <ostream>
#include <string>

namespace CryCC
{
namespace SubValue
{
namespace Valuedef
{

// class Value;

class SingleValue : public ValueContract
{
    using NativeTypeList = Cry::TypeTrait::TemplateHolder<int, char, float, double, bool>;
    // using ValueVariant = Cry::TypeTrait::TemplateHolder::template_apply<boost::variant>;

public:
    using value_category = ValueCategory::Plural;

    // Expose the value variants that this category can process.
    constexpr static const int value_variant_order = NativeTypeList::size;

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

    // Stream value to output stream.
	friend std::ostream& operator<<(std::ostream& os, const SingleValue&)
	{
		return os;
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
