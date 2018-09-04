// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/SubValue/ValueContract.h>
#include <CryCC/SubValue/NilType.h>

namespace CryCC::SubValue::Valuedef
{

// The offset value is a value pointing to an element in a composed
// value. The offset is always integral based.
class OffsetValue final : public AbstractValue<OffsetValue>
{
    size_t m_offset;
    IterableContract& m_iterValue;

    friend class ValueIterator;

public:
    using typdef_type = Typedef::NilType;
    using value_category = ValueCategory::Singular;
    using offset_type = decltype(m_offset);

    // Expose the value variants that this category can process.
    inline constexpr static const int value_variant_order = 0;
    // Unique value identifier.
    inline constexpr static const int value_category_identifier = 14;

	OffsetValue(const OffsetValue& other)
        : m_iterValue{ other.m_iterValue }
        , m_offset{ other.m_offset }
    {}
	OffsetValue(OffsetValue&&) = default;

	OffsetValue& operator=(const OffsetValue& other)
    {
        m_iterValue = other.m_iterValue;
        m_offset = other.m_offset;
    }
	OffsetValue& operator=(OffsetValue&&) = default;

    template<typename ValueCategoryType, typename = typename std::enable_if<IsValueIterable<ValueCategoryType>::value>::type>
    OffsetValue(ValueCategoryType& value, offset_type offset = 0)
        : m_iterValue{ value }
        , m_offset{ offset }
    {
    }

    inline size_t Offset() const { return m_offset; }

	//
	// Implement value category contract.
	//

	// NOTE: NilValue holds no data, thus serialization can be ignored.
	static void Serialize(const OffsetValue&, buffer_type&) {}
	// NOTE: NilValue holds no data, thus deserialization can be ignored.
	static void Deserialize(OffsetValue&, buffer_type&) {}

    // All the nil values are the same.
	bool operator==(const OffsetValue&) const { return true; }

    // Convert current value to string.
	std::string ToString() const { return "(ofs)"; }

    //
	// Arithmetic operators.
	//

	friend OffsetValue operator+(const OffsetValue&, const OffsetValue&) { throw InvalidValueArithmeticException{}; }
	friend OffsetValue operator-(const OffsetValue&, const OffsetValue&) { throw InvalidValueArithmeticException{}; }
	friend OffsetValue operator*(const OffsetValue&, const OffsetValue&) { throw InvalidValueArithmeticException{}; }
	friend OffsetValue operator/(const OffsetValue&, const OffsetValue&) { throw InvalidValueArithmeticException{}; }
	friend OffsetValue operator%(const OffsetValue&, const OffsetValue&) { throw InvalidValueArithmeticException{}; }
};

static_assert(std::is_copy_constructible<OffsetValue>::value, "OffsetValue !is_copy_constructible");
static_assert(std::is_move_constructible<OffsetValue>::value, "OffsetValue !is_move_constructible");
static_assert(std::is_copy_assignable<OffsetValue>::value, "OffsetValue !is_copy_assignable");
static_assert(std::is_move_assignable<OffsetValue>::value, "OffsetValue !is_move_assignable");

} // namespace CryCC::SubValue::Valuedef
