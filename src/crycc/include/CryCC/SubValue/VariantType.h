// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/SubValue/Typedef.h>

#include <vector>

namespace CryCC::SubValue::Typedef
{

// The vaiant holds an array of elements all of which can have a
// different type. The number of elements must match the number of
// types.
class VariantType : public AbstractType
{
public:
	// Unique type identifier.
	inline constexpr static const TypeVariation type_identifier = TypeVariation::VARIANT;

	VariantType(size_type elements, std::vector<BaseType> variantType);
	VariantType(size_type elements, std::vector<BaseType>&& variantType);

	// Return the size of the variant.
	inline size_type Order() const noexcept { return m_elements; }

	//
	// Implement abstract base type methods.
	//

	// Return type identifier.
	TypeVariation TypeId() const { return type_identifier; }
	// Return type name string.
	const std::string TypeName() const final;
	// Return native size.
	size_type UnboxedSize() const;
	// Test if types are equal.
	bool Equals(BasePointer) const;
	// Pack the type into a byte stream.
	buffer_type TypeEnvelope() const override;

private:
	size_type m_elements;
	std::vector<BaseType> m_elementTypes;
};

} // namespace CryCC::SubValue::Typedef
