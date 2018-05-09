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

template<typename Type>
class UniquePreservePtr : private std::unique_ptr<Type, void(*)(Type *)>
{
	using BaseType = std::unique_ptr<Type, void(*)(Type *)>;

	// Ingore deletion of object
	static void Deleter(Type *ptr)
	{
		CRY_UNUSED(ptr);
	}

public:
	UniquePreservePtr() = delete;
	UniquePreservePtr(nullptr_t)
	{
		this->reset(nullptr);
	}

	UniquePreservePtr(Type *ptr)
		: BaseType{ ptr, Deleter }
	{
	}

	template<typename PointerType>
	UniquePreservePtr(PointerType *ptr)
		: BaseType{ BaseType::pointer(ptr), Deleter }
	{
	}

	UniquePreservePtr(const UniquePreservePtr&) = delete;
	UniquePreservePtr(UniquePreservePtr&& other)
		: BaseType{ other.release(), Deleter }
	{
		other.reset();
	}

	//
	// Assignment operators
	//

	UniquePreservePtr& operator=(const UniquePreservePtr&) = delete;
	UniquePreservePtr& operator=(UniquePreservePtr&& other) noexcept
	{
		this->reset(other.release());
		other.reset();
		return (*this);
	}

	//
	// Object access
	//

	typename BaseType::pointer operator->()  const noexcept { return BaseType::get(); }
	typename BaseType::pointer operator*()  const noexcept { return BaseType::get(); }
	typename BaseType::pointer Get() const noexcept {return BaseType::get(); }
};

} // namespace Detail
