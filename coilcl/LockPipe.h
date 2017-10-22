#pragma once

#include <deque>
#include <stack>

typedef size_t pipe_state;

template<typename _Ty, typename _Container = std::deque<_Ty>>
class LockPipe
{
	typedef LockPipe<_Ty, _Container> _Myty;

public:
	typedef _Container cointainer_type;
	typedef typename _Container::value_type value_type;
	typedef typename _Container::size_type size_type;
	typedef typename _Container::reference reference;
	typedef typename _Container::const_reference const_reference;

	LockPipe()
		: c{}
	{
	}

	explicit LockPipe(const _Container& _Contain)
		: c{ _Contain }
	{
	}

	explicit LockPipe(_Container&& _Contain)
		: c{ std::move(_Contain) }
	{
	}

	LockPipe(const _Myty& other)
		: c{ cont }
	{
	}

	LockPipe(_Myty&& other)
		: c{ std::move(cont) }
	{
	}

	template<class _Alloc, class = std::enable_if_t<std::uses_allocator<_Container, _Alloc>::value>>
	explicit LockPipe(const _Alloc& alloc)
		: c{ alloc }
	{
	}

	template<class _Alloc, class = std::enable_if_t<std::uses_allocator<_Container, _Alloc>::value>>
	LockPipe(const _Container& _Contain, const _Alloc& alloc)
		: c{ _Contain, alloc }
	{
	}

	template<class _Alloc, class = std::enable_if_t<std::uses_allocator<_Container, _Alloc>::value>>
	LockPipe(_Container&& _Contain, const _Alloc& alloc)
		: c{ std::move(_Contain), alloc }
	{
	}

	template<class _Alloc, class = std::enable_if_t<std::uses_allocator<_Container, _Alloc>::value>>
	LockPipe(const _Myty& other, const _Alloc& alloc)
		: c{ other.c, alloc }
	{
	}

	template<class _Alloc, class = std::enable_if_t<std::uses_allocator<_Container, _Alloc>::value>>
	LockPipe(_Myty&& other, const _Alloc& alloc)
		: c{ std::move(other.c), alloc }
	{
	}

	void clear()
	{
		c.clear();
	}

	auto empty(bool skipLock = false) const
	{
		if (!l.empty() && !skipLock) {
			return (c.begin() + l.top()) == c.end();
		}

		return c.empty();
	}

	size_type size(bool skipLock = false) const
	{
		if (!l.empty() && !skipLock) {
			return (c.size() - l.top());
		}

		return c.size();
	}

	void push(const value_type& _Val)
	{
		c.push_back(_Val);
		index++;
	}

	void push(value_type&& _Val)
	{
		c.push_back(std::move(_Val));
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
		noexcept(_Is_nothrow_swappable<_Container>::value)
	{
		std::swap(c, other.c);
	}

protected:
	_Container c;
	std::stack<pipe_state> l;
	pipe_state index = 0;
};
