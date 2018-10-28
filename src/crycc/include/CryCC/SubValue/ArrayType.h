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

// Array of elements.
class ArrayType : public AbstractType
{
public:
	// Unique type identifier.
	inline constexpr static const TypeVariation type_identifier = TypeVariation::ARRAY;

	ArrayType(size_t elements, InternalBaseType& arrayType);
	ArrayType(size_t elements, InternalBaseType&& arrayType);

	template<auto Elements>
	ArrayType(InternalBaseType&& arrayType)
		: ArrayType{ Elements, std::move(arrayType) }
	{
	}

	// Return the size of the array.
	inline size_t Order() const noexcept { return m_elements; }
	// Return array base type.
	inline InternalBaseType Type() const { return m_elementType; }

	//
	// Implement type category contract.
	//

	// Convert array type into data stream.
	static void Serialize(const ArrayType&, buffer_type&);
	// Convert data stream into array type.
	static void Deserialize(ArrayType&, buffer_type&);

	// Compare to other BuiltinType.
	bool operator==(const ArrayType&) const { return true; }

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
	size_t m_elements;
	InternalBaseType m_elementType;
};

} // namespace CryCC::SubValue::Typedef
