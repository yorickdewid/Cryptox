// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/Program/Symbol.h>

#include <iostream>

namespace CryCC
{
namespace Program
{

#ifdef CRY_DEBUG
void SymbolMap::Print() const
{
	for (const auto& node : (*this)) {
		std::cout << "symbol: " << node.first << std::endl;
	}
}
#endif // CRY_DEBUG

void SymbolMap::Insert(const symbol_type& symbol)
{
	this->insert(symbol);
}

void SymbolMap::Insert(symbol_type&& symbol)
{
	this->emplace(std::move(symbol));
}

bool SymbolMap::Contains(const symbol_type::first_type& name) const noexcept
{
	return this->find(name) != this->end();
}

SymbolMap::symbol_type::second_type SymbolMap::GetNode(const symbol_type::first_type& name) const noexcept
{
	return this->find(name)->second;
}

SymbolMap& SymbolMap::operator<<(const symbol_type& symbol)
{
	this->Insert(symbol);
	return (*this);
}

} // namespace Program
} // namespace CryCC
