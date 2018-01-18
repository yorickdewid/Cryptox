// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <string>
#include <vector>

namespace ProjectBase
{

class Blob
{
public:
	Blob() = default;

	Blob(const std::string& name, size_t size = 0U)
		: origName{ name }
		, m_size{ size }
	{
	}

	Blob(const char name[], size_t size = 0U)
		: origName{ name }
		, m_size{ size }
	{
	}

	std::string Name() const
	{
		return origName;
	}

	size_t Size() const
	{
		return m_size;
	}

protected:
	std::string origName;
	size_t m_size;
};

} // namespace ProjectBase
