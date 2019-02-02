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

// Type definition is a named alias to another internal type
// structure. All type properties as passed through to the
// encapsulated type.
class TypedefType : public AbstractType
{
public:
	// Unique type identifier.
	inline constexpr static const TypeVariation type_identifier = TypeVariation::TYPEDEF;

	TypedefType(const std::string& name, InternalBaseType& nativeType);
	TypedefType(const std::string& name, InternalBaseType&& nativeType);
	TypedefType(buffer_type&);

	// Return type referenced base type.
	inline InternalBaseType MarkType() const noexcept { return m_resolveType; }
	// Return alias type name.
	inline std::string Name() const noexcept { return m_name; }

	//
	// Implement type category contract.
	//

protected:
	// Convert array type into data stream.
	void Pack(buffer_type&) const override;
	// Convert data stream into array type.
	void Unpack(buffer_type&) override;

	//
	// Implement abstract base type methods.
	//

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
	// Compare to other TypedefType.
	bool operator==(const TypedefType&) const { return true; }

private:
	std::string m_name;
	BaseType m_resolveType;
};

} // namespace CryCC::SubValue::Typedef
