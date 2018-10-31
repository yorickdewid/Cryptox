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
#include <Cry/ByteStream.h>

// Language includes.
#include <array>
#include <vector>

//TODO:
// - serialize/deserialize
// - equal ops

namespace CryCC::SubValue::Typedef
{

class TypeFacade;
class AbstractType;

//TODO: obsolete
using BaseType = std::shared_ptr<AbstractType>;
using InternalBaseType = std::shared_ptr<AbstractType>;

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

// Abstract type base class.
//
// Each internal type must inherit form this abstract type base and
// implement the mandatory methods. The type systemd is setup for
// extension so that new types can be made available to the subvalue
// package without changing the depended structures. Types should only
// hold type specific information, and must not include any value.
class AbstractType
{
public:
	using size_type = size_t;
	using buffer_type = Cry::ByteStream::VectorStream;

	// Storage class specifier.
	enum class StorageClassSpecifier
	{
		NONE_T,
		AUTO_T,
		STATIC_T,
		EXTERN_T,
		TYPEDEF_T,
		REGISTER_T,
	};

	// Type qualifier.
	enum class TypeQualifier
	{
		NONE_T,
		CONST_T,
		VOLATILE_T,
	};

	template<typename ContainerType, size_type Size>
	class StaticArray final
	{
	public:
		using InternalArray = std::array<ContainerType, Size>;
		using self_type = StaticArray<ContainerType, Size>;
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

		// Stream array into bytestream.
		friend AbstractType::buffer_type& operator<<(AbstractType::buffer_type& os, const self_type& array)
		{
			os << array.m_offset;
			os << static_cast<int>(Size);
			for (const auto& item : array.m_array) {
				os << item;
			}
			return os;
		}

		// Retrieve array from bytestream.
		friend AbstractType::buffer_type& operator>>(AbstractType::buffer_type& os, self_type& array)
		{
			os >> array.m_offset;
			int inputSize{ 0 };
			os >> inputSize;
			if (inputSize != Size) {
				CryImplExcept(); //TODO: booboo!
			}
			for (auto& item : array.m_array) {
				os >> item;
			}
			return os;
		}

	private:
		InternalArray m_array = InternalArray{};
		size_type m_offset{ 0 };
	};

	using Qualifiers = StaticArray<TypeQualifier, 2>;

public:
	// Convert abstract type into data stream.
	static void Serialize(const AbstractType&, buffer_type&);
	// Convert data stream into abstract type.
	static void Deserialize(AbstractType&, buffer_type&);

public:
	//
	// Abstract methods.
	//

	virtual TypeVariation TypeId() const = 0;
	virtual const std::string ToString() const = 0;
	virtual bool AllowCoalescence() const { return false; }
	virtual void Consolidate(InternalBaseType&) {};
	virtual size_type UnboxedSize() const = 0;
	virtual bool Equals(InternalBaseType*) const = 0;

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

	virtual void Pack(buffer_type&) const;
	virtual void Unpack(buffer_type&);

protected:
	bool m_isInline{ false };
	bool m_isSensitive{ false };
	StorageClassSpecifier m_storageClass = StorageClassSpecifier::NONE_T;
	Qualifiers m_typeQualifier = { TypeQualifier::NONE_T, TypeQualifier::NONE_T };
};

} // namespace CryCC::SubValue::Typedef
