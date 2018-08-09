// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/Memory.h>

#include <stack>

namespace CryCC
{
namespace AST
{

template<typename BaseTy, typename Container = std::deque<std::shared_ptr<BaseTy>>>
class ASTState
{
public:
	using container_type = Container;
	using allocator_type = typename Container::allocator_type;
	using value_type = typename Container::value_type;
	using size_type = typename Container::size_type;
	using difference_type = typename Container::difference_type;
	using reference = typename Container::reference;
	using const_reference = typename Container::const_reference;
	using pointer = typename Container::pointer;
	using const_pointer = typename Container::const_pointer;

public:
	using iterator = typename Container::iterator;
	using const_iterator = typename Container::const_iterator;
	using reverse_iterator = typename Container::reverse_iterator;
	using const_reverse_iterator = typename Container::const_reverse_iterator;

public:
	ASTState() = default;

	//
	// Iterator interfaces.
	//

	iterator begin() noexcept { return m_mementoList.begin(); }
	const_iterator begin() const noexcept { return m_mementoList.cbegin(); }
	iterator end() noexcept { return m_mementoList.end(); }
	const_iterator end() const noexcept { return m_mementoList.cend(); }

	//
	// Iterator interfaces.
	//

	reference front() { return m_mementoList.front(); }
	const_reference front() const { return m_mementoList.front(); }
	reference back() { return m_mementoList.back(); }
	const_reference back() const { return m_mementoList.back(); }

	// FUTURE: friend?
	inline auto Alteration() const { return m_mementoList.size(); };
	inline auto HasAlteration() const { return !m_mementoList.empty(); };

	//
	// Capacity operations.
	//

	size_type size() const noexcept { return m_mementoList.size(); }
	bool empty() const noexcept { return m_mementoList.empty(); }

	template<typename Type>
	void Bump(Type objectCpyState)
	{
		std::unique_ptr<Type> origPtr{ new Type{ objectCpyState } };

		auto ptrBase = Cry::StaticUniquePointerCast<BaseTy>(std::move(origPtr));
		m_mementoList.push_back(std::move(ptrBase));
	}

private:
	Container m_mementoList;
};

} // namespace AST
} // namespace CryCC
