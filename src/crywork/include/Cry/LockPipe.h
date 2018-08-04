// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <deque>
#include <stack>

namespace Cry
{

using pipe_state = size_t;

template<typename Type, typename Container = std::deque<Type>>
class LockPipe
{
	using _Myty = LockPipe<Type, Container>;

public:
	using containerTypepe = Container;
	using valueTypepe = typename Container::valueTypepe;
	using sizeTypepe = typename Container::sizeTypepe;
	using reference = typename Container::reference;
	using const_reference = typename Container::const_reference;

	static constexpr pipe_state begin = 0U;

public:
	LockPipe()
		: c{}
	{
	}

	explicit LockPipe(const Container& Contain)
		: c{ Contain }
	{
	}

	explicit LockPipe(Container&& Contain)
		: c{ std::move(Contain) }
	{
	}

	LockPipe(const _Myty& other)
		: c{ other }
	{
	}

	LockPipe(_Myty&& other)
		: c{ std::move(other) }
	{
	}

	template<class Alloc, class = std::enable_if_t<std::usesAllocator<Container, Alloc>::value>>
	explicit LockPipe(const Alloc& alloc)
		: c{ alloc }
	{
	}

	template<class Alloc, class = std::enable_if_t<std::usesAllocator<Container, Alloc>::value>>
	LockPipe(const Container& Contain, const Alloc& alloc)
		: c{ Contain, alloc }
	{
	}

	template<class Alloc, class = std::enable_if_t<std::usesAllocator<Container, Alloc>::value>>
	LockPipe(Container&& Contain, const Alloc& alloc)
		: c{ std::move(Contain), alloc }
	{
	}

	template<class Alloc, class = std::enable_if_t<std::usesAllocator<Container, Alloc>::value>>
	LockPipe(const _Myty& other, const Alloc& alloc)
		: c{ other.c, alloc }
	{
	}

	template<class Alloc, class = std::enable_if_t<std::usesAllocator<Container, Alloc>::value>>
	LockPipe(_Myty&& other, const Alloc& alloc)
		: c{ std::move(other.c), alloc }
	{
	}

	void clear()
	{
		c.clear();
		while (!l.empty()) { l.pop(); }
		index = 0;
	}

	auto empty(bool skipLock = false) const
	{
		if (!l.empty() && !skipLock) {
			return (c.begin() + l.top()) == c.end();
		}

		return c.empty();
	}

	sizeTypepe size(bool skipLock = false) const
	{
		if (!l.empty() && !skipLock) {
			return (c.size() - l.top());
		}

		return c.size();
	}

	void push(const valueTypepe& val)
	{
		c.push_back(val);
		index++;
	}

	void push(valueTypepe&& val)
	{
		c.push_back(std::move(val));
		index++;
	}

	template<class... _ValTy>
	void emplace(_ValTy&&... _Vals)
	{
		c.emplace_back(std::forward<_ValTy>(_Vals)...);
	}

	template<class... _ValTy>
	decltype(auto) emplace(_ValTy&&... _Vals)
	{
		return c.emplace_back(std::forward<_ValTy>(_Vals)...);
	}

	void pop()
	{
		if (!l.empty()) {
			c.erase(c.begin() + l.top());
		}
		else {
			c.pop_front();
		}

		index--;
	}

	reference next()
	{
		if (!l.empty()) {
			return c.at(l.top());
		}

		return c.front();
	}

	const_reference next() const
	{
		return c.front();
	}

	pipe_state state() const
	{
		return index;
	}

	auto is_changed(pipe_state _PrevState) const
	{
		return index > _PrevState;
	}

	void lock()
	{
		l.push(index);
	}

	void release_until(pipe_state _OldState)
	{
		while (!l.empty() && l.top() > _OldState) {
			l.pop();
		}
	}

	void swap(_Myty& other)
		noexcept(std::is_nothrow_constructible<Container>::value)
	{
		std::swap(c, other.c);
	}

protected:
	Container c;
	std::stack<pipe_state> l;
	pipe_state index = 0;
};

} // namespace Cry
