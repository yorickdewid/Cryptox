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

// FUTURE: Not fully implemented yet. Should not be used at this point.
class PointerType : public AbstractType
{
public:
	// Unique type identifier.
	inline constexpr static const TypeVariation type_identifier = TypeVariation::POINTER;

	PointerType(InternalBaseType& nativeType);
	PointerType(InternalBaseType&& nativeType);
	PointerType(buffer_type&);

	// Return pointer type.
	inline InternalBaseType Get() const { return m_ptrType; }

	//
	// Implement type category contract.
	//

protected:
	// Convert builtin type into data stream.
	void Pack(buffer_type&) const override;
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
	bool operator==(const PointerType&) const { return true; }

private:
	InternalBaseType m_ptrType;
};

} // namespace CryCC::SubValue::Typedef
