#pragma once

#include <utility>

template<typename _Ty = int>
struct _SourceLocationImpl : public std::pair<_Ty, _Ty>
{
	using value_type = _Ty;
	using _Myty = SourceLocation<_Ty>;
	using _Mybase = std::pair<_Ty, _Ty>;

public:
	SourceLocation(const value_type _Line, const value_type _Col)
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

	value_type Line() const
	{
		return first;
	}

	value_type Column() const
	{
		return second;
	}
};

typedef _SourceLocationImpl<> SourceLocation
