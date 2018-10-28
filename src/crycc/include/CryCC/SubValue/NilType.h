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

// The Nil type is a type to test the completeness fo type system.
// It has limited use in most languages and should be used with
// causion. The Nil type is not the allocated type for uninitialized
// values.
class NilType : public AbstractType
{
public:
	// Unique type identifier.
	inline constexpr static const TypeVariation type_identifier = TypeVariation::NIL;

	//
	// Implement type category contract.
	//

	// Convert builtin type into data stream.
	static void Serialize(const NilType&, buffer_type&);
	// Convert data stream into builtin type.
	static void Deserialize(NilType&, buffer_type&);

	// Compare to other NilType.
	bool operator==(const NilType&) const { return true; }

	//
	// Implement abstract base type methods.
	//

	// Return type identifier.
	TypeVariation TypeId() const { return type_identifier; }
	// Return type name string.
	const std::string ToString() const final;
	// Return native size.
	size_type UnboxedSize() const;
	// Test if types are equal.
	bool Equals(InternalBaseType*) const;
	// Pack the type into a byte stream.
	buffer_type TypeEnvelope() const override;
};

} // namespace CryCC::SubValue::Typedef
