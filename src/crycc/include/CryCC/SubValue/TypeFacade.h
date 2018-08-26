// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/SubValue/Typedef.h>

#include <Cry/Serialize.h>

#include <vector>

namespace CryCC
{
namespace SubValue
{
namespace Typedef
{

class TypeFacade
{
	BaseType m_type; //TODO: Remove pointer if possible at all
	size_t m_ptrCount{ 0 }; //TODO: remove
	size_t m_arrayElement{ 0 }; //TODO: remove

public:
	using base_type = decltype(m_type);
	using typedef_type = typename decltype(m_type)::element_type;
	using typedef_pointer = std::add_pointer<typedef_type>::type;
	using size_type = size_t;

	TypeFacade() = default; //FUTURE: maybe remove
	TypeFacade(const base_type&); //TODO: no reference, only move, copy

	// Check if type is initialized.
	inline bool HasValue() const noexcept { return m_type != nullptr; }

	//
	// Pointer operations.
	//

	inline bool IsPointer() const noexcept { return m_ptrCount > 0; }
	inline size_type PointerCount() const noexcept { return m_ptrCount; }
	inline void SetPointer(size_type ptrCount) noexcept { m_ptrCount = ptrCount; }

	//
	// Array operations.
	//

	inline bool IsArray() const noexcept { return m_arrayElement > 0; }
	inline size_type ArraySize() const noexcept { return m_arrayElement; }
	inline void SetArraySize(size_type element) noexcept { m_arrayElement = element; }

	// Single type native allocation size.
	Typedef::BaseType::element_type::size_type Size() const { return m_type->UnboxedSize(); }
	// Size of entire value object in allocation space.
	Typedef::BaseType::element_type::size_type ValuedSize() const
	{
		return IsArray() ? Size() * ArraySize() : Size();
	}

	// Concat type base name and pointer counter for convenience.
	std::string TypeName() const;

	// Access native type base.
	typedef_pointer operator->() const
	{
		if (!HasValue()) { return nullptr; }
		return m_type.get();
	}

	// Return the underlaying type. If the cast type is invalid for the type
	// zero is returned. The caller must check the value before use.
	template<typename CastType>
	auto DataType() const { return std::dynamic_pointer_cast<CastType>(m_type); }
	auto BaseType() const { return m_type; }

	// Convert type into data stream.
	static void Serialize(const TypeFacade&, Cry::ByteArray&);
	// Convert data stream into type.
	static void Deserialize(TypeFacade&, Cry::ByteArray&);

	//TODO: REMOVE: FIXME: DEPRECATED
	const std::type_info& Type() const
	{
		if (!HasValue()) { return typeid(nullptr); }
		return typeid(*m_type.get());
	}

	// Comparison equal operator.
	bool operator==(const TypeFacade&) const;
	// Comparison not equal operator.
	bool operator!=(const TypeFacade&) const;

private:
	std::string PointerName() const;
};

} // namespace Typedef
} // namespace SubValue
} // namespace CryCC

namespace Util
{

using namespace CryCC::SubValue::Typedef;

bool IsVoid(const TypeFacade&) noexcept;
bool IsIntegral(const TypeFacade&) noexcept;
bool IsFloatingPoint(const TypeFacade&) noexcept;
bool IsArray(const TypeFacade&) noexcept;
bool IsEnum(const TypeFacade&) noexcept;
bool IsStruct(const TypeFacade&) noexcept;
bool IsUnion(const TypeFacade&) noexcept;
bool IsClass(const TypeFacade&) noexcept;
bool IsRecord(const TypeFacade&) noexcept;
bool IsPointer(const TypeFacade&) noexcept;
bool IsInline(const TypeFacade&) noexcept;
bool IsSensitive(const TypeFacade&) noexcept;
bool IsStatic(const TypeFacade&) noexcept;
bool IsExtern(const TypeFacade&) noexcept;
bool IsRegister(const TypeFacade&) noexcept;
bool IsConst(const TypeFacade&) noexcept;
bool IsVolatile(const TypeFacade&) noexcept;
bool IsSigned(const TypeFacade&) noexcept;
bool IsUnsigned(const TypeFacade&) noexcept;

//
// Helpers to create types.
//

inline auto MakeBuiltinType(BuiltinType::Specifier specifier)
{
	return std::make_shared<BuiltinType>(specifier);
}
inline auto MakeRecordType(const std::string& name, RecordType::Specifier specifier)
{
	return std::make_shared<RecordType>(name, specifier);
}
inline auto MakeTypedefType(const std::string& name, BaseType& type)
{
	return std::make_shared<TypedefType>(name, type);
}
inline auto MakeTypedefType(const std::string& name, TypeFacade type)
{
	return std::make_shared<TypedefType>(name, type.BaseType());
}
inline auto MakeVariadicType()
{
	return std::make_shared<VariadicType>();
}
inline auto MakePointerType(BaseType& type)
{
	return std::make_shared<PointerType>(type);
}
inline auto MakePointerType(TypeFacade type)
{
	return std::make_shared<PointerType>(type.BaseType());
}

// Create type definition based on byte array.
BaseType MakeType(std::vector<uint8_t>&&);

} // namespace Util
