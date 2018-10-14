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

namespace CryCC::SubValue::Valuedef
{

// A reference value contains another value.
class ReferenceValue final : public AbstractValue<ReferenceValue>
{
    std::unique_ptr<Value> m_refValue;

public:
    using typdef_type = nullptr_t;
    using value_category = ValueCategory::Singular;

    // Expose the value variants that this category can process.
    inline constexpr static const int value_variant_order = 0;
    // Unique value identifier.
    inline constexpr static const int value_category_identifier = 9;

	ReferenceValue(const ReferenceValue&);
	ReferenceValue(ReferenceValue&&) = default;

	ReferenceValue& operator=(const ReferenceValue&);
	ReferenceValue& operator=(ReferenceValue&&) = default;

	ReferenceValue(Value&&);
	explicit ReferenceValue(buffer_type&);

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

	//
	// Arithmetic operators.
	//

	ReferenceValue& operator++();
	ReferenceValue& operator--();
	ReferenceValue operator++(int);
	ReferenceValue operator--(int);

	friend ReferenceValue operator+(const ReferenceValue&, const ReferenceValue&);
	friend ReferenceValue operator-(const ReferenceValue&, const ReferenceValue&);
	friend ReferenceValue operator*(const ReferenceValue&, const ReferenceValue&);
	friend ReferenceValue operator/(const ReferenceValue&, const ReferenceValue&);
	friend ReferenceValue operator%(const ReferenceValue&, const ReferenceValue&);
};

static_assert(std::is_copy_constructible_v<ReferenceValue>, "ReferenceValue !is_copy_constructible");
static_assert(std::is_move_constructible_v<ReferenceValue>, "ReferenceValue !is_move_constructible");
static_assert(std::is_copy_assignable_v<ReferenceValue>, "ReferenceValue !is_copy_assignable");
static_assert(std::is_move_assignable_v<ReferenceValue>, "ReferenceValue !is_move_assignable");

} // namespace CryCC::SubValue::Valuedef
