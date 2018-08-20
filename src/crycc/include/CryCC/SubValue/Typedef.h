// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/Cry.h>
#include <Cry/Except.h>

#include <cassert>
#include <array>
#include <string>
#include <memory>
#include <vector>
#include <list>
#include <bitset>

namespace CryCC
{
namespace SubValue
{
namespace Typedef
{

class TypeFacade;
class TypedefBase;

using BaseType = std::shared_ptr<Typedef::TypedefBase>;
using BaseType2 = std::shared_ptr<TypeFacade>;
using BasePointer = Typedef::TypedefBase*;

// Each internal type must inherit form this abstract type base and
// implement the mandatory methods. The type systemd is setup for
// extension so that new types can be made available to the subvalue
// package without changing the depended structures. Types should only
// hold type specific information, and must not include any value.
class TypedefBase
{
public:
	using size_type = size_t;
	using buffer_type = std::vector<uint8_t>;

	// Envelope helper to identify type. For every specialization a
	// type variation must be defined in the base class. The variation
	// is primarily used for envelope operations. If the type system
	// is extended a new variation must be appended to this enum.
	enum class TypeVariation : uint8_t
	{
		INVAL = 0,
		BUILTIN = 100,
		RECORD,
		TYPEDEF,
		VARIADIC,
		POINTER,
		ARRAY,
	};

	// Storage class specifier.
	enum class StorageClassSpecifier
	{
		// TODO: rename to use _T
		NONE,
		AUTO,
		STATIC,
		EXTERN,
		TYPEDEF,
		REGISTER,
	};

	// Type qualifier.
	enum class TypeQualifier
	{
		// TODO: rename to use _T
		NONE,
		CONST_T,
		VOLATILE,
	};

	template<typename ContainerType, size_type Size>
	class StaticArray final
	{
	public:
		using InternalArray = std::array<ContainerType, Size>;
		using value_type = typename InternalArray::value_type;
		using reference = typename InternalArray::reference;
		using const_reference = typename InternalArray::const_reference;
		using iterator = typename InternalArray::iterator;
		using const_iterator = typename InternalArray::const_iterator;
		using reverse_iterator = typename InternalArray::reverse_iterator;
		using const_reverse_iterator = typename InternalArray::const_reverse_iterator;

		StaticArray() = default;
		StaticArray(std::initializer_list<TypeQualifier> list)
		{
			for (const auto& value : list) {
				m_array.fill(value);
			}
		}

		bool Full() const noexcept { return m_offset >= Size; }
		bool Empty() const noexcept { return !m_offset; }

		iterator begin() { return m_array.begin(); }
		iterator end() { return m_array.end(); }
		const_iterator begin() const noexcept { return m_array.begin(); }
		const_iterator end() const noexcept { return m_array.end(); }
		const_iterator cbegin() const noexcept { return m_array.cbegin(); }
		const_iterator cend() const noexcept { return m_array.cend(); }

		const_reference operator[](size_type idx) const
		{
			return m_array[idx];
		}

		// Push item on the array.
		void PushBack(value_type tq)
		{
			if (!Full()) {
				m_array[m_offset++] = tq;
			}
		}

		// Push item on the array.
		void PushBack(value_type&& tq)
		{
			if (!Full()) {
				m_array[m_offset++] = std::move(tq);
			}
		}

	private:
		InternalArray m_array = InternalArray{};
		size_type m_offset{ 0 };
	};

	using Qualifiers = StaticArray<TypeQualifier, 2>;

public:
	//
	// Abstract methods.
	//

	virtual int TypeId() const = 0;
	virtual const std::string TypeName() const = 0;
	virtual bool AllowCoalescence() const { return false; }
	virtual void Consolidate(BaseType&) {};
	virtual size_type UnboxedSize() const = 0;
	virtual bool Equals(BasePointer) const = 0;
	virtual buffer_type TypeEnvelope() const;

	//
	// Type specifier inputs.
	//

	virtual void SetStorageClass(StorageClassSpecifier storageClass) noexcept { m_storageClass = storageClass; }
	virtual void SetQualifier(TypeQualifier typeQualifier) { m_typeQualifier.PushBack(typeQualifier); }
	virtual void SetInline() noexcept { m_isInline = true; }
	virtual void SetSensitive() noexcept { m_isSensitive = true; }

	//
	// Stringify type name.
	//

	const std::string StorageClassName() const;
	const std::string QualifierName() const;

	//
	// Additional type specifiers.
	//

	inline StorageClassSpecifier StorageClass() const noexcept { return m_storageClass; }
	inline Qualifiers TypeQualifiers() const noexcept { return m_typeQualifier; }
	inline bool IsInline() const noexcept { return m_isInline; }
	inline bool IsSensitive() const noexcept { return m_isSensitive; }

protected:
	bool m_isInline{ false };
	bool m_isSensitive{ false };
	StorageClassSpecifier m_storageClass = StorageClassSpecifier::NONE;
	Qualifiers m_typeQualifier = { TypeQualifier::NONE, TypeQualifier::NONE };
};

constexpr uint8_t SetInteralType(TypedefBase::TypeVariation type)
{
	return static_cast<uint8_t>(type);
}

#define REGISTER_TYPE(t)\
	const uint8_t m_c_internalType = SetInteralType(TypedefBase::TypeVariation::t); \
	uint8_t TypeIdentifier() const noexcept { return m_c_internalType; }

// Builtin types.
class BuiltinType : public TypedefBase
{
	// Additional type options.
	enum
	{
		IS_SIGNED,
		IS_UNSIGNED,
		IS_SHORT,
		IS_LONG,
		IS_LONG_LONG,
		IS_COMPLEX,
		IS_IMAGINARY,
	};

	REGISTER_TYPE(BUILTIN);
	std::bitset<8> m_typeOptions;

private:
	// If specifier matches a type option, set the option bit
	// and default the type to integer.
	void SpecifierToOptions();

public:
	//TODO: Add intmax_t, uintmax_t & others
	enum class Specifier
	{
		// TODO: rename to use _T
		VOID_T,
		CHAR,
		SHORT,
		INT,
		LONG,
		SIGNED,
		UNSIGNED,
		FLOAT,
		DOUBLE,
		BOOL,
	};

public:
	BuiltinType(Specifier specifier);

	//
	// Test type options.
	//

	inline auto Unsigned() const { return m_typeOptions.test(IS_UNSIGNED); }
	inline auto Signed() const { return !Unsigned(); }
	inline auto Short() const { return m_typeOptions.test(IS_SHORT); }
	inline auto Long() const { return m_typeOptions.test(IS_LONG); }
	inline auto Complex() const { return m_typeOptions.test(IS_COMPLEX); }
	inline auto Imaginary() const { return m_typeOptions.test(IS_IMAGINARY); }

	// Return the type specifier.
	Specifier TypeSpecifier() const { return m_specifier; }

	//
	// Implement abstract base type methods.
	//

	// Return type identifier.
	int TypeId() const { return TypeIdentifier(); }
	// Return type name string.
	const std::string TypeName() const;
	// If any type options are set, allow type coalescence.
	bool AllowCoalescence() const override { return m_typeOptions.any(); }
	// Return native size.
	size_type UnboxedSize() const;
	// Test if types are equal.
	bool Equals(BasePointer) const;
	// Pack the type into a byte stream.
	buffer_type TypeEnvelope() const override;
	// Consolidate multiple types into one.
	void Consolidate(BaseType& type) override;

private:
	Specifier m_specifier;
};

// Record types are types that consist of multiple types mapped to a name.
class RecordType : public TypedefBase
{
	REGISTER_TYPE(RECORD);

public:
	enum class Specifier
	{
		STRUCT,
		UNION,
		CLASS,
	};

public:
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
	int TypeId() const { return TypeIdentifier(); }
	// Return type name string.
	const std::string TypeName() const;
	// Return native size.
	size_type UnboxedSize() const;
	// Test if types are equal.
	bool Equals(BasePointer) const;
	// Pack the type into a byte stream.
	buffer_type TypeEnvelope() const override;

private:
	std::string m_name;
	Specifier m_specifier;
	std::vector<std::pair<std::string, BaseType2>> m_fields;
};

class TypedefType : public TypedefBase
{
	REGISTER_TYPE(TYPEDEF);
	std::string m_name;
	BaseType m_resolveType; //TODO: should be BaseType2, really?

public:
	TypedefType(const std::string& name, BaseType& nativeType);

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
};

class VariadicType : public TypedefBase
{
	REGISTER_TYPE(VARIADIC);

public:
	// Return type identifier.
	int TypeId() const { return TypeIdentifier(); }
	const std::string TypeName() const final { return "..."; }

	size_type UnboxedSize() const
	{
		throw Cry::Except::UnsupportedOperationException{ "VariadicType::UnboxedSize" };
	}

	bool Equals(BasePointer other) const
	{
		return dynamic_cast<VariadicType*>(other) != nullptr;
	}

	buffer_type TypeEnvelope() const override;
};

//TODO: How about no?
class PointerType : public TypedefBase
{
	REGISTER_TYPE(POINTER);

public:
	// Return type identifier.
	int TypeId() const { return TypeIdentifier(); }
	const std::string TypeName() const final { return "(ptr)"; }
	size_type UnboxedSize() const { return sizeof(int); } //TODO: not quite

	bool Equals(BasePointer other) const
	{
		return dynamic_cast<PointerType*>(other) != nullptr;
	}

	buffer_type TypeEnvelope() const override;
};

} // namespace Typedef
} // namespace SubValue
} // namespace CryCC
