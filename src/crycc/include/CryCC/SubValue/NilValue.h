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
#include <CryCC/SubValue/NilType.h>

namespace CryCC::SubValue::Valuedef
{

class NilValue final : public AbstractValue<NilValue>
{
public:
    using typdef_type = Typedef::NilType;
    using value_category = ValueCategory::Singular;

    // Expose the value variants that this category can process.
    inline constexpr static const int value_variant_order = 0;
    // Unique value identifier.
    inline constexpr static const int value_category_identifier = 0;

	//
	// Implement value category contract.
	//

	// NOTE: NilValue holds no data, thus serialization can be ignored.
	static void Serialize(const NilValue&, buffer_type&) {}
	// NOTE: NilValue holds no data, thus deserialization can be ignored.
	static void Deserialize(NilValue&, buffer_type&) {}

    // All the nil values are the same.
	bool operator==(const NilValue&) const { return true; }

    // Convert current value to string.
	std::string ToString() const { return "(nil)"; }

	//
	// Arithmetic operators.
	//

	friend NilValue operator+(const NilValue&, const NilValue&) { throw InvalidValueArithmeticException{}; }
	friend NilValue operator-(const NilValue&, const NilValue&) { throw InvalidValueArithmeticException{}; }
	friend NilValue operator*(const NilValue&, const NilValue&) { throw InvalidValueArithmeticException{}; }
	friend NilValue operator/(const NilValue&, const NilValue&) { throw InvalidValueArithmeticException{}; }
	friend NilValue operator%(const NilValue&, const NilValue&) { throw InvalidValueArithmeticException{}; }
};

static_assert(sizeof(NilValue) == sizeof(AbstractValue<NilValue>), "NilValue should not hold data");
static_assert(std::is_copy_constructible<NilValue>::value, "NilValue !is_copy_constructible");
static_assert(std::is_move_constructible<NilValue>::value, "NilValue !is_move_constructible");
static_assert(std::is_copy_assignable<NilValue>::value, "NilValue !is_copy_assignable");
static_assert(std::is_move_assignable<NilValue>::value, "NilValue !is_move_assignable");

} // namespace CryCC::SubValue::Valuedef
