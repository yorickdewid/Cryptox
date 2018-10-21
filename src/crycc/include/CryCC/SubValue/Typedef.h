// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

// Framework includes.
#include <Cry/Cry.h>
#include <Cry/Except.h>
#include <Cry/Types.h> // << Cry::Byte

// Language includes.
#include <cassert>
#include <array>
#include <string>
#include <memory>
#include <vector>
#include <list>
#include <bitset>

namespace CryCC::SubValue::Typedef
{

class TypeFacade;
class AbstractType;

//TODO: obsolete
using BaseType = std::shared_ptr<AbstractType>;
using InternalBaseType = std::shared_ptr<AbstractType>;
//using BaseType2 = std::shared_ptr<TypeFacade>;
using BasePointer = AbstractType*;

// Envelope helper to identify type. For every specialization a
// type variation must be defined in the base class. The variation
// is primarily used for envelope operations. If the type system
// is extended a new variation must be appended to this enum.
enum class TypeVariation
{
	INVAL = 0,
	BUILTIN = 100,
	RECORD,
	TYPEDEF,
	VARIADIC,
	POINTER,
	ARRAY,
	VARIANT,
	NIL,
};

// Each internal type must inherit form this abstract type base and
// implement the mandatory methods. The type systemd is setup for
// extension so that new types can be made available to the subvalue
// package without changing the depended structures. Types should only
// hold type specific information, and must not include any value.
class AbstractType
{
public:
	using size_type = size_t;
	using buffer_type = std::vector<uint8_t>;

	// Storage class specifier.
	enum class StorageClassSpecifier //TODO: add _T
	{
		// TODO: rename to use _T
		NONE,
		AUTO,
		STATIC,
		EXTERN,
		TYPEDEF,
		REGISTER,
	};

	// Type qualifier. //TODO: add _T
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

	virtual TypeVariation TypeId() const = 0;
	virtual const std::string TypeName() const = 0; //TODO: ToString();
	virtual bool AllowCoalescence() const { return false; }
	virtual void Consolidate(InternalBaseType&) {};
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

//constexpr Cry::Byte SetInteralType(AbstractType::TypeVariation type)
//{
//	return static_cast<Cry::Byte>(type);
//}
//
//#define REGISTER_TYPE(t)\
//	const Cry::Byte m_c_internalType = SetInteralType(AbstractType::TypeVariation::t); \
//	inline Cry::Byte TypeIdentifier() const noexcept { return m_c_internalType; }

} // namespace CryCC::SubValue::Typedef
