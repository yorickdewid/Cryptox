// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/AST/ASTNode.h>

#include <Cry/Cry.h>

#include <map>

namespace CryCC
{
namespace Program
{

struct SymbolMap : private std::map<std::string, AST::ASTNodeType>
{
	using symbol_type = std::pair<std::string, AST::ASTNodeType>;
	using self_type = std::map<std::string, AST::ASTNodeType>;
	using key_type = typename self_type::key_type;
	using mapped_type = typename self_type::mapped_type;
	using key_compare = typename self_type::key_compare;
	using value_compare = typename self_type::value_compare;
	using value_type = typename self_type::value_type;
	using allocator_type = typename self_type::allocator_type;
	using size_type = typename self_type::size_type;
	using difference_type = typename self_type::difference_type;
	using pointer = typename self_type::pointer;
	using const_pointer = typename self_type::const_pointer;
	using reference = typename self_type::reference;
	using const_reference = typename self_type::const_reference;
	using iterator = typename self_type::iterator;
	using const_iterator = typename self_type::const_iterator;
	using reverse_iterator = typename self_type::reverse_iterator;
	using const_reverse_iterator = typename self_type::const_reverse_iterator;

	//
	// Capacity operations.
	//

    size_t Count() const noexcept { return this->size(); }
    bool Empty() const noexcept { return this->empty(); }
	
#ifdef CRY_DEBUG
	void Print() const;
#endif // CRY_DEBUG

	// Insert symbol into symbol map.
	void Insert(const symbol_type&);
	// Emplace symbo object in map.
	void Insert(symbol_type&&);
	// Check if symbol map contains key.
	bool Contains(const symbol_type::first_type&) const noexcept;
	// Retrieve the corresponding node from the key.
	symbol_type::second_type GetNode(const symbol_type::first_type&) const noexcept;

	template<typename... ArgTypes>
	void Emplace(ArgTypes&&... args)
	{
		this->emplace(std::forward<ArgTypes>(args)...);
	}

	// Streaming insert operation.
	SymbolMap& operator<<(const symbol_type&);
};

} // namespace Program
} // namespace CryCC
