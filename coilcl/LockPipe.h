#pragma once

#include <deque>

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

	template<class _Alloc>
	//TODO: enable_if
	explicit LockPipe(const _Alloc& alloc)
		: c{ alloc }
	{
	}

	template<class _Alloc>
	LockPipe(const _Container& _Contain, const _Alloc& alloc)
		: c{_Contain, alloc}
	{
	}

	template<class _Alloc>
	LockPipe(_Container&& _Contain, const _Alloc& alloc)
		: c{ std::move(_Contain), alloc }
	{
	}

	template<class _Alloc>
	LockPipe(const _Myty& other, const _Alloc& alloc)
		: c{ other.c, alloc }
	{
	}

	template<class _Alloc>
	LockPipe(_Myty&& other, const _Alloc& alloc)
		: c{ std::move(other.c), alloc }
	{
	}

	bool empty() const
	{
		return c.empty();
	}

	size_type size() const
	{
		return c.size();
	}

	void push(const value_type& _Val)
	{
		c.push_back(_Val);
	}

	void push(value_type&& _Val)
	{
		c.push_back(std::move(_Val));
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

	/*void pop()
	{
		c.pop_back();
	}*/
	/*const_reference pop() const
	{ }*/

	/*reference pop()
	{*/
		//c.begin
		//std::move(c.end(),);

		//return c.back();
		//c.pop_back();
	//}

	reference top()
	{
		return c.back();
	}

	const_reference top() const
	{
		return c.back();
	}

	void swap(_Myty& other)
		noexcept(_Is_nothrow_swappable<_Container>::value)
	{
		std::swap(c, other.c);
	}

protected:
	_Container c;
	size_t index;
};
