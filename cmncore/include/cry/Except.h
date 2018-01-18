// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <stdexcept>

#include "Cry.h"

namespace Cry
{
namespace Except
{

class UnsupportedOperationException : public CSTD runtime_error
{
public:
	UnsupportedOperationException(const CSTD string& message) noexcept
		: CSTD runtime_error{ message }
	{
	}

	explicit UnsupportedOperationException(char const* const message) noexcept
		: CSTD runtime_error{ message }
	{
	}

	virtual const char *what() const noexcept
	{
		return CSTD runtime_error::what();
	}
};

class IncompatibleException : public CSTD runtime_error
{
public:
	IncompatibleException(const CSTD string& message) noexcept
		: CSTD runtime_error{ message }
	{
	}

	explicit IncompatibleException(char const* const message) noexcept
		: CSTD runtime_error{ message }
	{
	}

	virtual const char *what() const noexcept
	{
		return CSTD runtime_error::what();
	}
};

class NotImplementedException : public CSTD runtime_error
{
public:
	NotImplementedException(const CSTD string& message) noexcept
		: CSTD runtime_error{ message }
	{
	}

	explicit NotImplementedException(char const* const message) noexcept
		: CSTD runtime_error{ message }
	{
	}

	virtual const char *what() const noexcept
	{
		return CSTD runtime_error::what();
	}
};

} // namespace Except
} // namespace Cry
