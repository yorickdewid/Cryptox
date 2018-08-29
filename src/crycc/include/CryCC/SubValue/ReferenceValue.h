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

namespace CryCC
{
namespace SubValue
{
namespace Valuedef
{

// A reference value contains another value.
class ReferenceValue final : public AbstractValue<ReferenceValue>
{
    std::unique_ptr<Value2> m_refValue;

public:
    using typdef_type = nullptr_t;
    using value_category = ValueCategory::Singular;

    // Expose the value variants that this category can process.
    constexpr static const int value_variant_order = 0;
    // Unique value identifier.
    constexpr static const int value_category_identifier = 9;

	ReferenceValue(Value2&&);

	//
	// Implement value category contract.
	//

	// Convert reference value into data stream.
	static void Serialize(const ReferenceValue&, buffer_type&);
	// Convert data stream into reference value.
	static void Deserialize(ReferenceValue&, buffer_type&);

    // Compare to other ReferenceValue.
	bool operator==(const ReferenceValue& other) const;

    // Convert current value to string.
	std::string ToString() const;
};

} // namespace Valuedef
} // namespace SubValue
} // namespace CryCC
