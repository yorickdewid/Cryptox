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

	ArrayType(buffer_type&);

	// Return the size of the array.
	inline size_t Order() const noexcept { return m_elements; }
	// Return array base type.
	inline InternalBaseType Type() const { return m_elementType; }

	//
	// Implement type category contract.
	//

protected:
	// Convert array type into data stream.
	void Pack(buffer_type&) const override;
	// Convert data stream into array type.
	void Unpack(buffer_type&) override;

public:
	// Return type identifier.
	TypeVariation TypeId() const { return type_identifier; }
	// Return type name string.
	const std::string ToString() const final;
	// Return native size.
	size_type UnboxedSize() const;
	// Test if types are equal.
	bool Equals(InternalBaseType*) const;

	// TODO:
	// Compare to other ArrayType.
	bool operator==(const ArrayType&) const { return true; }

private:
	size_t m_elements;
	InternalBaseType m_elementType;
};

} // namespace CryCC::SubValue::Typedef
