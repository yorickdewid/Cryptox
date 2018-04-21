#pragma once

#include <utility>

namespace CoilCl
{
namespace Util
{
namespace Detail
{

template<typename Type = int, typename = typename std::enable_if<std::is_integral<Type>::value>::type>
struct SourceLocationImpl : public std::pair<Type, Type>
{
	using value_type = Type;
	using _Myty = SourceLocationImpl<Type>;
	using _Mybase = std::pair<Type, Type>;

	SourceLocationImpl() = default;
	inline SourceLocationImpl(const value_type Line, const value_type Col)
		: _Mybase{ Line, Col }
	{
	}

	SourceLocationImpl(const _Myty& Other)
		: _Mybase{ Other }
	{
	}

	SourceLocationImpl(_Myty&&)
		: _Mybase{ std::move(_Other) }
	{
	}

	SourceLocationImpl& operator=(const _Myty& _Other)
	{
		first = _Other.first;
		second = _Other.second;
		return (*this);
	}

	SourceLocationImpl& operator=(_Myty&& _Other)
	{
		first = std::move(_Other.first);
		second = std::move(_Other.second);
		return (*this);
	}

	value_type Line() const noexcept { return first; }
	value_type Column() const noexcept { return second; }
};

} // namespace Detail

using SourceLocation = Detail::SourceLocationImpl<>;

} // namespace Util
} // namespace CoilCl
