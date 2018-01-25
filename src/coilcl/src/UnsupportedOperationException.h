#pragma once

#include <stdexcept>

namespace CoilCl
{

class UnsupportedOperationException : public std::runtime_error
{
public:
	UnsupportedOperationException(const std::string& message) noexcept
		: std::runtime_error{ message }
	{
	}

	explicit UnsupportedOperationException(char const* const message) noexcept
		: std::runtime_error{ message }
	{
	}

	virtual const char *what() const noexcept
	{
		return std::runtime_error::what();
	}
};

} // namespace CoilCl