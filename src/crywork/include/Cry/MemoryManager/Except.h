// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <exception>

namespace Cry
{
namespace MemoryManager
{

struct OutOfMemoryException : public std::exception
{
	size_t m_allocatedSize;

public:
	OutOfMemoryException(size_t size)
		: m_allocatedSize{ size }
	{
	}

	virtual ~OutOfMemoryException() noexcept
	{
	}

	virtual char const *what() const noexcept override
	{
		return ""; //TODO
	}
};

} // namespace Cry
} // namespace MemoryManager
