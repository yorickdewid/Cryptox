#pragma once

#include <stdexcept>

class IncompatibleException : public std::runtime_error
{
public:
	IncompatibleException(const std::string& message) noexcept
		: std::runtime_error{ message }
	{
	}

	explicit IncompatibleException(char const* const message) noexcept
		: std::runtime_error{ message }
	{
	}

	virtual const char *what() const noexcept
	{
		return std::runtime_error::what();
	}
};
