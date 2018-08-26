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
#include <CryCC/SubValue/Valuedef.h>

#include <Cry/Cry.h>
#include <Cry/TypeTrait.h>
#include <Cry/Serialize.h>

namespace CryCC
{
namespace SubValue
{
namespace Valuedef
{

// A reference value contains another value.
class ReferenceValue final : public AbstractValue<ReferenceValue>
{
    std::unique_ptr<Value> m_refValue;

public:
    using typdef_type = nullptr_t;
    using value_category = ValueCategory::Singular;

    // Expose the value variants that this category can process.
    constexpr static const int value_variant_order = 0;
    // Unique value identifier.
    constexpr static const int value_category_identifier = 9;

	ReferenceValue(Value&& value)
        : m_refValue{ std::make_unique<Value>(std::move(value)) }
    {
    }

	//
	// Implement value category contract.
	//

	// Convert reference value into data stream.
	static void Serialize(const ReferenceValue&, buffer_type&) {}
	// Convert data stream into reference value.
	static void Deserialize(ReferenceValue&, buffer_type&) {}

    // All the nil values are the same.
	bool operator==(const ReferenceValue& other) const
    {
        return m_refValue == other.m_refValue;
    }

    // Convert current value to string.
	std::string ToString() const { return "(ref)"; }
};

} // namespace Valuedef
} // namespace SubValue
} // namespace CryCC
