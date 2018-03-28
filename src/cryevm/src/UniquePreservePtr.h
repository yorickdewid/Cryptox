// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Cry/Indep.h"

#include <memory>

namespace Detail
{

template<typename _Ty>
class UniquePreservePtr : private std::unique_ptr<_Ty, void(*)(_Ty *)>
{
	using _MyBase = std::unique_ptr<_Ty, void(*)(_Ty *)>;
	using _MyTy = UniquePreservePtr<_Ty>;

	// Ingore deletion of object
	static void Deleter(_Ty *ptr)
	{
		CRY_UNUSED(ptr);
	}

public:
	UniquePreservePtr() = delete;
	UniquePreservePtr(nullptr_t)
	{
		reset(nullptr);
	}

	UniquePreservePtr(_Ty *ptr)
		: _MyBase{ ptr, Deleter }
	{
	}

	template<typename _PTy>
	UniquePreservePtr(_PTy *ptr)
		: _MyBase{ _MyBase::pointer(ptr), Deleter }
	{
	}

	UniquePreservePtr(const UniquePreservePtr&) = delete;

	UniquePreservePtr(UniquePreservePtr&& other)
		: _MyBase{ other.release(), Deleter }
	{
		other.reset();
	}

	UniquePreservePtr& operator=(const UniquePreservePtr&) = delete;

	UniquePreservePtr& operator=(UniquePreservePtr&& other) noexcept
	{
		reset(other.release());
		other.reset();
		return (*this);
	}

	_MyBase::pointer Get() const noexcept
	{
		return _MyBase::get();
	}
};

} // namespace Detail
