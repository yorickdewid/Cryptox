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

using BaseType2 = std::shared_ptr<TypeFacade>;

// Record types are types that consist of multiple types mapped to a name.
class RecordType : public AbstractType
{
public:
	enum class Specifier
	{
		STRUCT,
		UNION,
		CLASS,
	};

public:
	// Unique type identifier.
	inline constexpr static const TypeVariation type_identifier = TypeVariation::RECORD;

	RecordType(const std::string& name, Specifier specifier = Specifier::STRUCT);
	//RecordType(const std::string& name, Specifier specifier, size_t elements, BaseType type);

	void AddField(const std::string& field, const BaseType2& type)
	{
		m_fields.push_back({ field, type });
	}

	void AddField(std::string&& field, BaseType2&& type)
	{
		m_fields.emplace_back(std::move(field), std::move(type));
	}

	inline bool IsAligned() const noexcept { return m_aligned; }
	inline bool IsAnonymous() const noexcept { return m_name.empty(); }
	inline std::string Name() const noexcept { return m_name; }
	inline size_t FieldSize() const noexcept { return m_fields.size(); }
	inline auto Fields() const noexcept { return m_fields; }

	// Return the record specifier.
	Specifier TypeSpecifier() const { return m_specifier; }

	//
	// Implement abstract base type methods.
	//

	// Return type identifier.
	TypeVariation TypeId() const { return type_identifier; }
	// Return type name string.
	const std::string ToString() const;
	// Return native size.
	size_type UnboxedSize() const;
	// Test if types are equal.
	bool Equals(InternalBaseType*) const;
	// Pack the type into a byte stream.
	buffer_type TypeEnvelope() const override;

private:
	bool m_aligned{ false };
	std::string m_name;
	Specifier m_specifier;
	std::vector<std::pair<std::string, BaseType2>> m_fields;
};
} // namespace CryCC::SubValue::Typedef
