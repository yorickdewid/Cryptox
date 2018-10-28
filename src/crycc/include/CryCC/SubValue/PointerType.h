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

class PointerType : public AbstractType
{
public:
	// Unique type identifier.
	inline constexpr static const TypeVariation type_identifier = TypeVariation::POINTER;

	PointerType(BaseType& nativeType);
	PointerType(BaseType&& nativeType);

	// Return pointer type.
	inline BaseType Get() const { return m_ptrType; }

	//
	// Implement type category contract.
	//

	// Convert pointer type into data stream.
	static void Serialize(const PointerType&, buffer_type&);
	// Convert data stream into pointer type.
	static void Deserialize(PointerType&, buffer_type&);

	// Compare to other BuiltinType.
	bool operator==(const PointerType&) const { return true; }

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
	//buffer_type TypeEnvelope() const override;

private:
	BaseType m_ptrType;
};

} // namespace CryCC::SubValue::Typedef
