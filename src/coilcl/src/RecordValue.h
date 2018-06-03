// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <vector>
#include <memory>

namespace CoilCl
{
namespace Valuedef
{

class Value;

//template<class Type>
//class ucp_ptr {
//	std::unique_ptr<Type> m_ptr;
//
//public:
//	ucp_ptr() = default;
//	ucp_ptr(const ucp_ptr& other)
//	{
//		//m_ptr = std::move(other.m_ptr);
//	};
//	ucp_ptr(ucp_ptr&&) = default;
//	
//	ucp_ptr(Type *ptr)
//		: m_ptr{ ptr }
//	{
//	};
//	ucp_ptr(std::unique_ptr<Type>&& other)
//		: m_ptr(std::move(other))
//	{
//	};
//
//	Type& operator*() { return m_ptr.get(); }
//	const Type& operator*() const { return m_ptr.get(); }
//
//	Type* operator->() { return m_ptr.get(); }
//	const Type* operator->() const { return m_ptr.get(); }
//};

//static_assert(std::is_copy_constructible<ucp_ptr<int>>::value, "!is_copy_constructible");
//static_assert(std::is_move_constructible<ucp_ptr<int>>::value, "!is_move_constructible");

class RecordValue
{
	std::vector<std::pair<std::string, std::shared_ptr<Value>>> m_fields;

public:
	void AddField(std::pair<std::string, std::shared_ptr<Value>>&& val)
	{
		m_fields.push_back(std::move(val));
	}

	size_t Size() const noexcept { return m_fields.size(); }
};

} // namespace Valuedef
} // namespace CoilCl
