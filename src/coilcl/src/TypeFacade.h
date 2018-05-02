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
	Typedef::BaseType m_type;
	size_t m_ptrCount = 0;

public:
	//FUTURE: maybe remove
	TypeFacade() = default;
	TypeFacade(const Typedef::BaseType& type)
		: m_type{ type }
	{
	}

	// Fetch type information
	inline auto HasValue() const noexcept { return m_type != nullptr; }
	inline auto IsPointer() const noexcept { return m_ptrCount > 0; }
	inline auto PointerCount() const noexcept { return m_ptrCount; }
	inline void SetPointer(size_t ptrCount) { m_ptrCount = ptrCount; }
	inline auto Size() const { return m_type->UnboxedSize(); }

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

inline bool IsStaticType(const AST::TypeFacade& typeFacace)
{
	return typeFacace->StorageClass() == Typedef::TypedefBase::StorageClassSpecifier::STATIC;
}
template<typename InternalType>
inline bool IsStaticType(const InternalType& type)
{
	return type->StorageClass() == Typedef::TypedefBase::StorageClassSpecifier::STATIC;
}

inline bool IsExternType(const AST::TypeFacade& typeFacace)
{
	return typeFacace->StorageClass() == Typedef::TypedefBase::StorageClassSpecifier::EXTERN;
}
template<typename InternalType>
inline bool IsExternType(const InternalType& type)
{
	return type->StorageClass() == Typedef::TypedefBase::StorageClassSpecifier::EXTERN;
}

} // namespace Util
} // namespace CoilCl
