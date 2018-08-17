// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/Cry.h>

#include <stdexcept>

namespace Cry
{
namespace Except
{

class UnsupportedOperationException : public CSTD runtime_error
{
public:
	UnsupportedOperationException(const CSTD string& message) noexcept
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

class IncompatibleException : public CSTD runtime_error
{
public:
	IncompatibleException(const CSTD string& message) noexcept
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

class NotImplementedException : public std::runtime_error
{
public:
	NotImplementedException(const std::string& message) noexcept
		: std::runtime_error{ message }
	{
	}

	explicit NotImplementedException(char const* const message) noexcept
		: std::runtime_error{ message }
	{
	}

	virtual const char *what() const noexcept
	{
		return std::runtime_error::what();
	}
};

class KeyNotFoundException : public std::runtime_error
{
public:
	KeyNotFoundException(const std::string& message) noexcept
		: std::runtime_error{ message }
	{
	}

	explicit KeyNotFoundException(char const* const message) noexcept
		: std::runtime_error{ message }
	{
	}

	virtual const char *what() const noexcept
	{
		return std::runtime_error::what();
	}
};

} // namespace Except
} // namespace Cry
