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

class NilValue final : public AbstractValue<NilValue>
{
public:
    using typdef_type = nullptr_t;
    using value_category = ValueCategory::Singular;

    // Expose the value variants that this category can process.
    constexpr static const int value_variant_order = 0;
    // Unique value identifier.
    constexpr static const int value_category_identifier = 9;

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
};

static_assert(sizeof(NilValue) == sizeof(AbstractValue<NilValue>), "NilValue should not hold data");

} // namespace Valuedef
} // namespace SubValue
} // namespace CryCC
