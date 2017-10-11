#pragma once

#include <utility>

template<typename _Ty = int>
struct SourceLocation : public std::pair<_Ty, _Ty>
{
	using type = _Ty;
	using _Myty = SourceLocation<_Ty>;
	typedef std::pair<_Ty, _Ty> _Mybase;

public:
	SourceLocation(const type _Line, const type _Col)
		: _Mybase{ _Line, _Col }
	{
	}

	SourceLocation()
		: _Mybase{}
	{
	}

	SourceLocation(const _Myty& _Other)
		: _Mybase{ _Other }
	{
	}

	SourceLocation(_Myty&&)
		: _Mybase{ std::move(_Other) }
	{
	}

	SourceLocation& operator=(const _Myty& _Other)
	{
		first = _Other.first;
		second = _Other.second;
		return (*this);
	}

	SourceLocation& operator=(_Myty&& _Other)
	{
		first = std::move(_Other.first);
		second = std::move(_Other.second);
		return (*this);
	}

	type line() const
	{
		return first;
	}

	type column() const
	{
		return second;
	}
};
