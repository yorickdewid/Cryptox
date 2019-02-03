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

	struct FieldType : protected std::pair<std::string, InternalBaseType>
	{
		using base_type = std::pair<std::string, InternalBaseType>;

		inline const auto Name() const noexcept { return first; }
		inline const auto Type() const noexcept { return second; }

		FieldType(const typename base_type::first_type& name
			, const typename base_type::second_type& type)
			: base_type{ name, type }
		{
		}
		FieldType(typename base_type::first_type&& name
			, const typename base_type::second_type&& type)
			: base_type{ std::move(name), std::move(type) }
		{
		}
	};

	using VectorFieldList = std::vector<FieldType>;
	//using LinkedFieldList = std::list<FieldType>;
	//using OrderFieldList = std::set<FieldType>;

public:
	// Unique type identifier.
	inline constexpr static const TypeVariation type_identifier = TypeVariation::RECORD;

	RecordType(Specifier specifier = Specifier::STRUCT);
	RecordType(const std::string& name, Specifier specifier = Specifier::STRUCT);
	RecordType(buffer_type&);

	void AddField(const std::string& field, const InternalBaseType& type);
	void AddField(std::string&& field, InternalBaseType&& type);
	void AddField(FieldType&& field);

	inline bool IsAligned() const noexcept { return m_aligned; }
	inline bool IsAnonymous() const noexcept { return m_name.empty(); }
	inline std::string Name() const noexcept { return m_name; }
	inline size_t FieldSize() const noexcept { return m_fields.size(); }
	inline auto Fields() const noexcept { return m_fields; }

	// Return the record specifier.
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
	// Return native size.
	size_type UnboxedSize() const;
	// Test if types are equal.
	bool Equals(InternalBaseType*) const;

	// TODO:
	// Compare to other RecordType.
	bool operator==(const RecordType&) const { return true; }

private:
	bool m_aligned{ false };
	std::string m_name;
	Specifier m_specifier;
	VectorFieldList m_fields;
};

} // namespace CryCC::SubValue::Typedef
