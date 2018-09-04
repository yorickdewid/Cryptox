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

namespace CryCC::SubValue::Valuedef
{

// A pointer value points to a memory chunk containing data.
class PointerValue final : public AbstractValue<PointerValue>
{
public:
    using typdef_type = nullptr_t;
    using value_category = ValueCategory::Singular;

    // Expose the value variants that this category can process.
    inline constexpr static const int value_variant_order = 0;
    // Unique value identifier.
    inline constexpr static const int value_category_identifier = 12;

	//
	// Implement value category contract.
	//

	// NOTE: PointerValue holds no data, thus serialization can be ignored.
	static void Serialize(const PointerValue&, buffer_type&) {}
	// NOTE: PointerValue holds no data, thus deserialization can be ignored.
	static void Deserialize(PointerValue&, buffer_type&) {}

    // TODO:
	bool operator==(const PointerValue&) const { return true; }

    // TODO:
	std::string ToString() const { return "(ptr)"; }

	//TODO:
	//
	// Arithmetic operators.
	//

	friend PointerValue operator+(const PointerValue&, const PointerValue&) { return PointerValue{}; }
	friend PointerValue operator-(const PointerValue&, const PointerValue&) { return PointerValue{}; }
	friend PointerValue operator*(const PointerValue&, const PointerValue&) { return PointerValue{}; }
	friend PointerValue operator/(const PointerValue&, const PointerValue&) { return PointerValue{}; }
	friend PointerValue operator%(const PointerValue&, const PointerValue&) { return PointerValue{}; }
};

static_assert(std::is_copy_constructible<PointerValue>::value, "PointerValue !is_copy_constructible");
static_assert(std::is_move_constructible<PointerValue>::value, "PointerValue !is_move_constructible");
static_assert(std::is_copy_assignable<PointerValue>::value, "PointerValue !is_copy_assignable");
static_assert(std::is_move_assignable<PointerValue>::value, "PointerValue !is_move_assignable");

} // namespace CryCC::SubValue::Valuedef
