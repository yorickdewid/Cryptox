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

namespace CryCC
{
namespace SubValue
{
namespace Typedef
{

// The vaiant holds an array of elements all of which can have a
// different type. The number of elements must match the number of
// types.
class VariantType : public TypedefBase
{
	REGISTER_TYPE(ARRAY);

public:
	VariantType(size_t elements, std::vector<BaseType> variantType);
	VariantType(size_t elements, std::vector<BaseType>&& variantType);

	// Return the size of the variant.
	inline size_t Order() const noexcept { return m_elements; }

	//
	// Implement abstract base type methods.
	//

	// Return type identifier.
	int TypeId() const { return TypeIdentifier(); }
	// Return type name string.
	const std::string TypeName() const final;
	// Return native size.
	size_type UnboxedSize() const;
	// Test if types are equal.
	bool Equals(BasePointer) const;
	// Pack the type into a byte stream.
	buffer_type TypeEnvelope() const override;

private:
	size_t m_elements;
	std::vector<BaseType> m_elementTypes;
};

} // namespace Typedef
} // namespace SubValue
} // namespace CryCC
