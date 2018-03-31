// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <stack>

namespace CoilCl
{

// All of the user defined structues *must* inherit this interface
// so various components can pack and unpack the structure.
struct UserDataInterface
{
	virtual void Serialize() = 0;
	virtual void Deserialize() = 0;
};

class UserDataWrapper : public UserDataInterface
{
	bool isFreeRequired = false;
	UserDataInterface *m_ptr;

public:
	UserDataWrapper(UserDataInterface *ptr, bool managed = false)
		: m_ptr{ ptr }
		, isFreeRequired{ managed }
	{
	}

	UserDataWrapper(intptr_t *ptr)
		: m_ptr{ reinterpret_cast<UserDataInterface *>(ptr) }
	{
	}

	UserDataWrapper(void *ptr)
		: m_ptr{ reinterpret_cast<UserDataInterface *>(ptr) }
	{
	}

	UserDataWrapper(const UserDataWrapper&) = default; //TODO
	UserDataWrapper(UserDataWrapper&&) = default;

	UserDataWrapper& operator=(const UserDataWrapper&) = default; //TODO
	UserDataWrapper& operator=(UserDataWrapper&&) = default;

	// Serialize passthrough
	virtual void Serialize() { m_ptr->Serialize(); }
	// Deserialize passthrough
	virtual void Deserialize() { m_ptr->Deserialize(); }

	// Access internal pointer
	inline UserDataInterface *operator*() const noexcept { return m_ptr; }
	inline UserDataInterface *operator->() const noexcept { return m_ptr; }
	inline UserDataInterface *get() const noexcept { return m_ptr; }

	// Release pointer to caller
	UserDataInterface *release() noexcept
	{
		auto tmpPtr = m_ptr;
		m_ptr = nullptr;
		return tmpPtr;
	}

	template<typename _Ty, typename = typename std::enable_if<std::is_base_of<UserDataInterface, _Ty>::value>::type>
	static UserDataWrapper Make()
	{
		return std::move(UserDataWrapper{ reinterpret_cast<UserDataInterface *>(new _Ty{}), true });
	}

	~UserDataWrapper()
	{
		// Delete pointer if required to free resources
		if (isFreeRequired && m_ptr) {
			delete m_ptr;
			m_ptr = nullptr;
		}
	}
};

} // namespace CoilCl
