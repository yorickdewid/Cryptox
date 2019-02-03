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

	VariantType(std::vector<InternalBaseType>& variantType);
	VariantType(std::vector<InternalBaseType>&& variantType);
	VariantType(std::initializer_list<InternalBaseType>&& variantType);
	VariantType(buffer_type&);

	// Return the size of the variant.
	inline size_type Order() const noexcept { return m_elementTypes.size(); }

	//
	// Implement type category contract.
	//

protected:
	// Convert builtin type into data stream.
	void Pack(buffer_type& buffer) const override;
	// Convert data stream into builtin type.
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
	// Compare to other BuiltinType.
	bool operator==(const VariantType&) const { return true; }

private:
	std::vector<InternalBaseType> m_elementTypes;
};

} // namespace CryCC::SubValue::Typedef
