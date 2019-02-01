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

class TypedefType : public AbstractType
{
public:
	// Unique type identifier.
	inline constexpr static const TypeVariation type_identifier = TypeVariation::TYPEDEF;

	TypedefType(const std::string& name, BaseType& nativeType);
	TypedefType(const std::string& name, BaseType&& nativeType);

	// Return type referenced base type.
	inline BaseType MarkType() const { return m_resolveType; }

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
	std::string m_name;
	BaseType m_resolveType;
};

} // namespace CryCC::SubValue::Typedef