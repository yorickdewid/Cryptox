// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <cstdint>

namespace CryCC
{
namespace SubValue
{

// All of the user defined structues *must* inherit this interface
// so that various components can pack and unpack the structure.
struct UserDataInterface
{
	using size_type = size_t;

	// Serialize the internal data structures.
	virtual void Serialize() = 0;
	// Deserialize the datastream into the data structures.
	virtual void Deserialize() = 0;
	// Retrieve the internal data size.
	virtual size_type Size() = 0;
};

//TODO: template + custom delete + unique_ptr, no UserDataInterface inherit
class UserDataPtr : public UserDataInterface
{
	bool isFreeRequired{ false };
	UserDataInterface *m_ptr{ nullptr };

public:
	UserDataPtr(UserDataInterface *ptr, bool managed = false)
		: m_ptr{ ptr }
		, isFreeRequired{ managed }
	{
	}

	UserDataPtr(intptr_t *ptr)
		: m_ptr{ reinterpret_cast<UserDataInterface *>(ptr) }
	{
	}

	UserDataPtr(void *ptr)
		: m_ptr{ reinterpret_cast<UserDataInterface *>(ptr) }
	{
	}

	UserDataPtr(const UserDataPtr&) = default; //TODO
	UserDataPtr(UserDataPtr&&) = default;

	UserDataPtr& operator=(const UserDataPtr&) = default; //TODO
	UserDataPtr& operator=(UserDataPtr&&) = default;

	// Serialize passthrough.
	virtual void Serialize() { m_ptr->Serialize(); }
	// Deserialize passthrough.
	virtual void Deserialize() { m_ptr->Deserialize(); }
	// Size passthrough.
	virtual size_type Size() { return m_ptr->Size(); }

	//
	// Access internal pointer.
	//

	inline UserDataInterface *operator*() const noexcept { return m_ptr; }
	inline UserDataInterface *operator->() const noexcept { return m_ptr; }
	inline UserDataInterface *Get() const noexcept { return m_ptr; }

	// Release pointer to caller.
	UserDataInterface *Release() noexcept
	{
		auto tmpPtr = m_ptr;
		m_ptr = nullptr;
		return tmpPtr;
	}

	virtual ~UserDataPtr()
	{
		// Delete pointer if required to free resources
		if (isFreeRequired && m_ptr) {
			delete m_ptr;
			m_ptr = nullptr;
		}
	}
};

} // namespace SubValue
} // namespace CryCC

namespace Util
{

using namespace CryCC::SubValue;

template<typename Type, typename = typename std::enable_if<std::is_base_of<UserDataInterface, Type>::value>::type>
static auto MakeUserDataPtr()
{
	return { reinterpret_cast<UserDataInterface *>(new Type{}), true };
}

} // namespace Util
