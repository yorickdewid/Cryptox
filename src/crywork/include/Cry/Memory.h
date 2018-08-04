// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Cry.h"

#include <boost/shared_ptr.hpp>

#include <memory>

namespace Cry
{

template<typename ToType, typename FromType>
std::unique_ptr<ToType> StaticUniquePointerCast(std::unique_ptr<FromType>&& old)
{
	return std::unique_ptr<ToType>{ static_cast<ToType*>(old.release()) };
}

template<class Type1, typename Type2>
constexpr Type1& side_cast(Type2 *opaquePtr) noexcept
{
	return static_cast<Type1&>(*static_cast<Type1*>(const_cast<typename std::remove_const<Type2>::type*>(opaquePtr)));
}

namespace
{

template<typename SharedPointer>
class Holder
{
	SharedPointer m_ptr;

public:
	Holder(const SharedPointer& p) : m_ptr(p) {}
	Holder(const Holder& other) = default;
	Holder(Holder&& other) = default;

	SharedPointer Get() const noexcept { return m_ptr; }

	void operator () (...) { m_ptr.reset(); }
};

} // namespace

// Convert boost shared pointer into std shared pointer.
template<typename Type>
std::shared_ptr<Type> MakeShared(const boost::shared_ptr<Type>& p)
{
	using HolderType = Holder<std::shared_ptr<Type>>;
	using WrapperType = Holder<boost::shared_ptr<Type>>;
	if (HolderType *holder = boost::get_deleter<HolderType, Type>(p)) {
		return holder->Get();
	}

	return { p.get(), WrapperType{ p } };
}

} // namespace Cry
