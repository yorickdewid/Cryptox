// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/SubValue/Typedef.h>

namespace CryCC::SubValue::Typedef
{

class VariadicType final : public AbstractType
{
public:
	// Unique type identifier.
	inline constexpr static const TypeVariation type_identifier = TypeVariation::VARIADIC;

	//
	// Implement type category contract.
	//

	// Convert builtin type into data stream.
	static void Serialize(const VariadicType&, buffer_type&);
	// Convert data stream into builtin type.
	static void Deserialize(VariadicType&, buffer_type&);

	//TODO:
	// Compare to other NilType.
	bool operator==(const VariadicType&) const { return true; }

	//
	// Implement abstract base type methods.
	//

	// Return type identifier.
	TypeVariation TypeId() const { return type_identifier; }
	// Return type name string.
	const std::string ToString() const;
	// Return native size.
	size_type UnboxedSize() const;
	// Test if types are equal.
	bool Equals(InternalBaseType*) const;
};

} // namespace CryCC::SubValue::Typedef
