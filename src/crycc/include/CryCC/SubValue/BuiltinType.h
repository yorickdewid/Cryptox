// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/SubValue/Typedef.h>

#include <Cry/Types.h>

#include <bitset>

namespace CryCC::SubValue::Typedef
{

// Builtin type structure.
//
// Represents a builtin type structure. Builtin types are types
// which are native to the system and have a predefined size
// in memory.
class BuiltinType : public AbstractType
{
	// Additional type options.
	enum
	{
		IS_SIGNED, //TODO: remove, obsolete
		IS_UNSIGNED, //TODO: remove, obsolete
		IS_SHORT, //TODO: remove, obsolete
		IS_LONG, //TODO: remove, obsolete
		IS_LONG_LONG, //TODO: remove, obsolete
		IS_COMPLEX,
		IS_IMAGINARY,
	};

	// If specifier matches a type option, set the option bit
	// and default the type to integer.
	void SpecifierToOptions();

public:
	enum class Specifier : Cry::Byte
	{
		VOID_T = 200,
		BOOL_T,
		CHAR_T,
		SIGNED_CHAR_T,
		UNSIGNED_CHAR_T,
		SHORT_T,
		UNSIGNED_SHORT_T,
		INT_T,
		UNSIGNED_INT_T,
		LONG_T,
		UNSIGNED_LONG_T,
		FLOAT_T,
		DOUBLE_T,
		LONG_DOUBLE_T,
		UNSIGNED_LONG_DOUBLE_T,
	};

public:
	// Unique type identifier.
	inline constexpr static const TypeVariation type_identifier = TypeVariation::BUILTIN;

	BuiltinType(Specifier specifier);
	BuiltinType(buffer_type&);

	//
	// Test type options.
	//

	inline bool Unsigned() const { return m_typeOptions.test(IS_UNSIGNED); }
	inline bool Signed() const { return !Unsigned(); }
	inline bool Short() const { return m_typeOptions.test(IS_SHORT); }
	inline bool Long() const { return m_typeOptions.test(IS_LONG); }
	inline bool Complex() const { return m_typeOptions.test(IS_COMPLEX); }
	inline bool Imaginary() const { return m_typeOptions.test(IS_IMAGINARY); }

	// Return the type specifier.
	Specifier TypeSpecifier() const { return m_specifier; }

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
	const std::string ToString() const;
	// If any type options are set, allow type coalescence.
	bool AllowCoalescence() const override { return m_typeOptions.any(); }
	// Return native size.
	size_type UnboxedSize() const;
	// Test if types are equal.
	bool Equals(const InternalBaseType&) const;
	// Consolidate multiple types into one.
	void Consolidate(InternalBaseType& type) override;

	// Compare to other BuiltinType.
	bool operator==(const BuiltinType&) const;
	// Compare to other BuiltinType.
	bool operator!=(const BuiltinType&) const;

private:
	Specifier m_specifier;
	std::bitset<8> m_typeOptions;
};

} // namespace CryCC::SubValue::Typedef
