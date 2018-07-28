// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Typedef.h"

#include <Cry/Serialize.h>

#include <vector>

namespace CoilCl
{
namespace Typedef
{

class TypeFacade
{
	Typedef::BaseType m_type; //TODO: Remove pointer if possible at all
	//Typedef::TypedefBase& m_type2;
	size_t m_ptrCount = 0;
	size_t m_arrayElement = 0;

public:
	//FUTURE: maybe remove
	TypeFacade() = default;
	TypeFacade(const Typedef::BaseType& type)
		: m_type{ type }
	{
	}

	//
	// Fetch type information
	//

	inline bool HasValue() const noexcept { return m_type != nullptr; }
	inline bool IsPointer() const noexcept { return m_ptrCount > 0; }
	inline size_t PointerCount() const noexcept { return m_ptrCount; }
	inline void SetPointer(size_t ptrCount) { m_ptrCount = ptrCount; }
	inline bool IsArray() const noexcept { return m_arrayElement > 0; }
	inline size_t ArraySize() const noexcept { return m_arrayElement; }
	inline void SetArraySize(size_t element) { m_arrayElement = element; }
	inline size_t Size() const { return m_type->UnboxedSize(); }

	// Concat type base name and pointer counter for convenience.
	std::string TypeName() const
	{
		if (!HasValue()) { return {}; }
		return m_type->TypeName() + PointerName();
	}

	// Access native type base.
	Typedef::TypedefBase *operator->() const
	{
		if (!HasValue()) { return nullptr; }
		return m_type.get();
	}

	// Return the underlaying type. If the cast type is invalid for the type
	// zero is returned. The caller must check the value before use.
	template<typename CastType>
	auto DataType() const { return std::dynamic_pointer_cast<CastType>(m_type); }

	// Convert type into data stream
	static void Serialize(int,const TypeFacade&, Cry::ByteArray&);
	// Convert data stream into type
	static void Deserialize(int,TypeFacade&, Cry::ByteArray&);

	//TODO: REMOVE: FIXME: DEPRECATED
	const std::type_info& Type() const
	{
		if (!HasValue()) { return typeid(nullptr); }
		return typeid(*m_type.get());
	}

	// Comparison equal operator
	bool operator==(const TypeFacade& other) const { return m_type->Equals(other.m_type.get()); }
	// Comparison not equal operator
	bool operator!=(const TypeFacade& other) const { return !m_type->Equals(other.m_type.get()); }

private:
	std::string PointerName() const;
};

} // namespace AST

namespace Util
{

using namespace Typedef;

inline bool IsVoid(const TypeFacade& other) noexcept
{
	const auto *builtin = dynamic_cast<BuiltinType *>(other.operator->());
	if (!builtin) { return false; }
	return builtin->TypeSpecifier() == BuiltinType::Specifier::VOID_T;
}
inline bool IsIntegral(const TypeFacade& other) noexcept
{
	const auto *builtin = dynamic_cast<BuiltinType *>(other.operator->());
	if (!builtin) { return false; }
	return builtin->TypeSpecifier() == BuiltinType::Specifier::INT;
}
inline bool IsFloatingPoint(const TypeFacade& other) noexcept
{
	const auto *builtin = dynamic_cast<BuiltinType *>(other.operator->());
	if (!builtin) { return false; }
	return builtin->TypeSpecifier() == BuiltinType::Specifier::FLOAT;
}
inline bool IsArray(const TypeFacade& other) noexcept
{
	return other.IsArray();
}
inline bool IsEnum(const TypeFacade& /*other*/) noexcept
{
	return false;
}
inline bool IsStruct(const TypeFacade& other) noexcept
{
	const auto *record = dynamic_cast<Typedef::RecordType *>(other.operator->());
	if (!record) { return false; }
	return record->TypeSpecifier() == Typedef::RecordType::Specifier::STRUCT;
}
inline bool IsUnion(const TypeFacade& other) noexcept
{
	const auto *record = dynamic_cast<Typedef::RecordType *>(other.operator->());
	if (!record) { return false; }
	return record->TypeSpecifier() == Typedef::RecordType::Specifier::UNION;
}
inline bool IsPointer(const TypeFacade& other) noexcept
{
	return other.IsPointer();
}
inline bool IsInline(const TypeFacade& other) noexcept
{
	const auto *builtin = dynamic_cast<Typedef::BuiltinType *>(other.operator->());
	if (!builtin) { return false; }
	return builtin->IsInline();
}
inline bool IsStatic(const TypeFacade& other) noexcept
{
	return other->StorageClass() == Typedef::TypedefBase::StorageClassSpecifier::STATIC;
}
inline bool IsExtern(const TypeFacade& other) noexcept
{
	return other->StorageClass() == Typedef::TypedefBase::StorageClassSpecifier::EXTERN;
}
inline bool IsRegister(const TypeFacade& other) noexcept
{
	return other->StorageClass() == Typedef::TypedefBase::StorageClassSpecifier::REGISTER;
}
inline bool IsConst(const TypeFacade& other) noexcept
{
	for (const auto& qualifier : other->TypeQualifiers()) {
		if (qualifier == Typedef::TypedefBase::TypeQualifier::CONST_T) {
			return true;
		}
	}
	return false;
}
inline bool IsVolatile(const TypeFacade& other) noexcept
{
	for (const auto& qualifier : other->TypeQualifiers()) {
		if (qualifier == Typedef::TypedefBase::TypeQualifier::VOLATILE) {
			return true;
		}
	}
	return false;
}
inline bool IsSigned(const TypeFacade& other) noexcept
{
	const auto *builtin = dynamic_cast<Typedef::BuiltinType *>(other.operator->());
	if (!builtin) { return false; }
	return builtin->Unsigned();
}
inline bool IsUnsigned(const TypeFacade& other) noexcept
{
	return !IsSigned(other);
}

//TODO: Why pointer? Reference possible?

//
// Helpers to create types
//

inline auto MakeBuiltinType(BuiltinType::Specifier specifier)
{
	return std::make_shared<BuiltinType>(specifier);
}
inline auto MakeRecordType(const std::string& name, RecordType::Specifier specifier)
{
	return std::make_shared<Typedef::RecordType>(name, specifier);
}
inline auto MakeTypedefType(const std::string& name, BaseType& type)
{
	return std::make_shared<Typedef::TypedefType>(name, type);
}
inline auto MakeVariadicType()
{
	return std::make_shared<Typedef::VariadicType>();
}
inline auto MakePointerType()
{
	return std::make_shared<Typedef::PointerType>();
}

// Create type definition based on byte array.
Typedef::BaseType MakeType(std::vector<uint8_t>&&);

} // namespace Util
} // namespace CoilCl
