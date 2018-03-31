// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Typedef.h"

#include <memory>
#include <string>

namespace CoilCl
{
namespace AST
{

class TypeFacade
{
	std::shared_ptr<Typedef::TypedefBase> m_type;
	size_t m_ptrCount = 0;

public:
	TypeFacade() = default;//TODO: Should be removed?
	
	TypeFacade(const std::shared_ptr<Typedef::TypedefBase>& type)
		: m_type{ type }
	{
	}

	// Fetch type information
	inline auto HasValue() const { return m_type != nullptr; }
	inline auto IsPointer() const { return m_ptrCount > 0; }
	inline void SetPointer(size_t ptrCount) { m_ptrCount = ptrCount; }
	inline auto Size() const { return m_type->UnboxedSize(); }

	// Concat type base name and pointer counter for convenience
	auto TypeName() const
	{
		return m_type->TypeName() + PointerName();
	}

	// Access native type base
	Typedef::TypedefBase *operator->() const
	{
		return m_type.get();
	}

	const std::type_info& Type() const
	{
		return typeid(*m_type.get());
	}

	bool operator==(const TypeFacade& other) const { return m_type->Equals(other.m_type.get()); }
	bool operator!=(const TypeFacade& other) const { return !m_type->Equals(other.m_type.get()); }

private:
	std::string PointerName() const
	{
		if (m_ptrCount == 0) {
			return "";
		}

		return " " + std::string(m_ptrCount, '*');
	}
};

} // namespace AST
} // namespace CoilCl