#pragma once

#include <utility>

namespace CoilCl
{
namespace Util
{
namespace Detail
{

template<typename Type = int, typename = typename std::enable_if<std::is_integral<Type>::value>::type>
struct SourceLocationImpl : private std::pair<Type, Type>
{
	using ValueType = Type;
	using SelfType = SourceLocationImpl<Type>;
	using BaseType = std::pair<Type, Type>;

	SourceLocationImpl() = default;
	inline SourceLocationImpl(const ValueType Line, const ValueType Col)
		: BaseType{ Line, Col }
	{
	}

	inline SourceLocationImpl(const SelfType& Other)
		: BaseType{ Other }
	{
	}

	inline SourceLocationImpl(SelfType&& other)
		: BaseType{ std::move(other) }
	{
	}

	SourceLocationImpl& operator=(const SelfType& other)
	{
		this->first = other.first;
		this->second = other.second;
		return (*this);
	}

	SourceLocationImpl& operator=(SelfType&& other)
	{
		this->first = std::move(other.first);
		this->second = std::move(other.second);
		return (*this);
	}

	//
	// Access location properties
	//

	ValueType Line() const noexcept { return this->first; }
	ValueType Column() const noexcept { return this->second; }
};

} // namespace Detail

using SourceLocation = Detail::SourceLocationImpl<>;
using SourceLocationLong = Detail::SourceLocationImpl<long long int>;

} // namespace Util
} // namespace CoilCl
