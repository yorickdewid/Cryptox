// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <stack>

namespace CryCC
{
namespace AST
{

template<typename _ToTy, typename _FromTy>
std::unique_ptr<_ToTy> static_unique_pointer_cast(std::unique_ptr<_FromTy>&& old)
{
	return std::unique_ptr<_ToTy>{ static_cast<_ToTy*>(old.release()) };
}

template<typename _BaseTy, typename _Container = std::deque<std::shared_ptr<_BaseTy>>>
class ASTState
{
public:
	using container_type = _Container;
	using allocator_type = typename _Container::allocator_type;
	using value_type = typename _Container::value_type;
	using size_type = typename _Container::size_type;
	using difference_type = typename _Container::difference_type;
	using reference = typename _Container::reference;
	using const_reference = typename _Container::const_reference;
	using pointer = typename _Container::pointer;
	using const_pointer = typename _Container::const_pointer;

public:
	using iterator = typename _Container::iterator;
	using const_iterator = typename _Container::const_iterator;
	using reverse_iterator = typename _Container::reverse_iterator;
	using const_reverse_iterator = typename _Container::const_reverse_iterator;

public:
	ASTState() = default;

	// Iterator interfaces
	iterator begin() noexcept { return m_mementoList.begin(); }
	const_iterator begin() const noexcept { return m_mementoList.cbegin(); }
	iterator end() noexcept { return m_mementoList.end(); }
	const_iterator end() const noexcept { return m_mementoList.cend(); }

	// Iterator interfaces
	reference front() { return m_mementoList.front(); }
	const_reference front() const { return m_mementoList.front(); }
	reference back() { return m_mementoList.back(); }
	const_reference back() const { return m_mementoList.back(); }

	// FUTURE: friend?
	inline auto Alteration() const { return m_mementoList.size(); };
	inline auto HasAlteration() const { return !m_mementoList.empty(); };

	template<typename _Ty>
	void Bump(_Ty objectCpyState)
	{
		std::unique_ptr<_Ty> origPtr{ new _Ty{ objectCpyState } };

		auto ptrBase = static_unique_pointer_cast<_BaseTy>(std::move(origPtr));

		m_mementoList.push_back(std::move(ptrBase));
	}

private:
	_Container m_mementoList;
};

} // namespace AST
} // namespace CryCC
