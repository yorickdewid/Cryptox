// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be
// copied and/or distributed without the express of the author.

#pragma once

#include <memory>

// The instrusive scoped pointer holds an unique pointer to the
// object. The instrusive scoped pointer cannot be copied, but
// the object it points to can. This deep copy will call the 
// copy constructor on the object and wrap it into a instrusive
// scoped pointer.
template<typename _Ty, typename _Dx = std::default_delete<_Ty>>
class IntrusiveScopedPtr
{
	std::unique_ptr<_Ty, _Dx> m_ptr;
	int copiesFromThis = 0;

	//static int totalRoamingObjects;

	using element_type = _Ty;
	using deleter_type = _Dx;
	using _Myty = IntrusiveScopedPtr<_Ty, _Dx>;

public:
	constexpr IntrusiveScopedPtr() noexcept = default;
	constexpr IntrusiveScopedPtr(nullptr_t) noexcept {};

	// Default initializer
	IntrusiveScopedPtr(_Ty *ptr)
		: m_ptr{ ptr }
	{
	}

	// Only allow special operator function move
	IntrusiveScopedPtr(const IntrusiveScopedPtr&) = delete;
	IntrusiveScopedPtr(IntrusiveScopedPtr&& other)
	{
		m_ptr = std::move(other.m_ptr);
	}

	// Forward default pointer operations
	auto get() const noexcept { return m_ptr.get(); }
	auto release() const noexcept { return m_ptr.release(); }

	// Forward default access operators
	auto operator->() { return (*m_ptr); }
	auto operator*() { return (*m_ptr); }

	IntrusiveScopedPtr& operator=(nullptr_t) noexcept
	{
		this->reset();
		return (*this);
	}

	IntrusiveScopedPtr& operator=(const IntrusiveScopedPtr&) = delete;
	IntrusiveScopedPtr& operator=(IntrusiveScopedPtr&& other) noexcept
	{
		if (this != std::addressof(other)) {
			this->reset(other.release());
		}

		return (*this);
	}

	explicit operator bool() const noexcept
	{
		return m_ptr.operator bool();
	}

	// Deep copy will copy the original data object and return a
	// new intrusive scoped pointer. This new intrusive scoped pointer
	// object has no relation to the original. Since the object is deep
	// copied, the instrusive scoped pointers share no object lifetime.
	_Myty deep_copy()
	{
		++copiesFromThis;

		_Ty *_ptr = m_ptr.get();

		return _Myty( _Ty(), _Dx );
	}

	void reset(_Ty ptr = _Ty()) noexcept { m_ptr.reset(ptr); }

	// Deep copy counters
	bool has_copies() const noexcept { return copiesFromThis > 0; }
	int count_copies() const noexcept { return copiesFromThis; }
};

namespace Cry
{

template<typename _Ty, typename... _ArgsTy>
inline IntrusiveScopedPtr<_Ty> MakeIntrusiveScoped(_ArgsTy&&... _Args)
{
	return (IntrusiveScopedPtr<_Ty>(new _Ty{ std::forward<_ArgsTy>(_Args)... }));
}

} // namespace Cry
