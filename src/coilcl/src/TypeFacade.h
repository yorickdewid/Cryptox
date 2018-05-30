// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Typedef.h"

#include <vector>

namespace CoilCl
{
namespace AST //TODO: part of Typedef
{

class TypeFacade
{
	Typedef::BaseType m_type; //TODO: Remove pointer if possible at all
	size_t m_ptrCount = 0;

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
	inline size_t Size() const { return m_type->UnboxedSize(); }

	// Concat type base name and pointer counter for convenience
	std::string TypeName() const
	{
		if (!HasValue()) { return {}; }
		return m_type->TypeName() + PointerName();
	}

	// Access native type base
	Typedef::TypedefBase *operator->() const
	{
		if (!HasValue()) { return nullptr; }
		return m_type.get();
	}

	// Convert type into data stream
	static void Serialize(const TypeFacade&, std::vector<uint8_t>&);
	// Convert data stream into type
	static void Deserialize(TypeFacade&, const std::vector<uint8_t>&);

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

template<typename InternalType>
inline bool IsTypeStatic(const InternalType& type)
{
	return type->StorageClass() == Typedef::TypedefBase::StorageClassSpecifier::STATIC;
}
template<typename InternalType>
inline bool IsTypeExtern(const InternalType& type)
{
	return type->StorageClass() == Typedef::TypedefBase::StorageClassSpecifier::EXTERN;
}
template<typename InternalType>
inline bool IsTypeRegister(const InternalType& type)
{
	return type->StorageClass() == Typedef::TypedefBase::StorageClassSpecifier::REGISTER;
}
template<typename InternalType>
inline bool IsTypeConst(const InternalType& type)
{
	for (const auto& qualifier : type->TypeQualifiers()) {
		if (qualifier == Typedef::TypedefBase::TypeQualifier::CONST_T) {
			return true;
		}
	}
	return false;
}
template<typename InternalType>
inline bool IsTypeVolatile(const InternalType& type)
{
	for (const auto& qualifier : type->TypeQualifiers()) {
		if (qualifier == Typedef::TypedefBase::TypeQualifier::VOLATILE) {
			return true;
		}
	}
	return false;
}
template<typename InternalType>
inline bool IsTypePointer(const InternalType& type)
{
	return type.IsPointer();
}

} // namespace Util
} // namespace CoilCl
