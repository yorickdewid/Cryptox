// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be
// copied and/or distributed without the express of the author.

#pragma once

#include "Cry/PolyConstructTrait.h"

#include <memory>

namespace CoilCl
{

// The instrusive scoped pointer holds an unique pointer to the
// object. The instrusive scoped pointer cannot be copied, but
// the object it points to can. This deep copy will call the 
// copy constructor on the object and wrap it into a instrusive
// scoped pointer.
template<typename _Ty, typename _Dx = std::default_delete<_Ty>>
class IntrusiveScopedPtr
{
	std::unique_ptr<_Ty, _Dx> m_ptr;

	using pointer_type = std::unique_ptr<_Ty, _Dx>;

#if (defined _DEBUG || defined DEBUG)
	int copiesFromThis = 0;
#endif

	using element_type = _Ty;
	using deleter_type = _Dx;
	using _Myty = IntrusiveScopedPtr<_Ty, _Dx>;

	template<typename _GTy>
	using _CtorCpyGuard = std::enable_if<!std::is_reference<_GTy>::value
		&& (std::is_copy_constructible<_Ty>::value || Cry::IsPolyconstructable<_Ty>::value)>;

	template<typename _GTy>
	using _CtorNCpyGuard = std::enable_if<!std::is_reference<_GTy>::value
		&& (!std::is_copy_constructible<_GTy>::value && !Cry::IsPolyconstructable<_Ty>::value)>;

	template<bool b>
	struct CopySelector
	{
		// Default object copy
		static _Ty *Impl(pointer_type& ptr)
		{
			if (!ptr) { return nullptr; }
			return new _Ty{ (*ptr) };

			static_assert(std::is_copy_constructible<_Ty>::value, "");
		}
	};

	template<>
	struct CopySelector<true>
	{
		// Copy via poly constructor contract
		static _Ty *Impl(pointer_type& ptr)
		{
			return dynamic_cast<_Ty*>(ptr->Copy());
		}
	};

	// Trait algorithm selector
	_Ty *ExplicitCopy()
	{
		return CopySelector<Cry::IsPolyconstructable<_Ty>::value>::Impl(m_ptr);
	}

public:
	constexpr IntrusiveScopedPtr() noexcept = default;
	constexpr IntrusiveScopedPtr(nullptr_t) noexcept {};

	// Default initializer, passed pointer is freed upon scope exit
	template<typename = typename _CtorCpyGuard<_Ty>::type>
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

	// Qualified unique pointers that can be copied are eligible
	template<typename _OrigTy, typename = typename _CtorCpyGuard<_Ty>::type>
	IntrusiveScopedPtr(std::unique_ptr<_OrigTy>&& ptr)
		: m_ptr{ ptr.release() }
	{
	}

	// Qualified unique pointers that cannot be copied supply a copy predicate
	template<typename _OrigTy, typename _CpyPredTy, typename = typename _CtorNCpyGuard<_Ty>::type>
	IntrusiveScopedPtr(std::unique_ptr<_OrigTy>&& ptr, _CpyPredTy pred)
		: m_ptr{ ptr.release() }
	{
	}

	// Forward default pointer operations
	auto get() const noexcept { return m_ptr.get(); }
	auto release() noexcept { return m_ptr.release(); }

	// Forward default access operators
	auto operator->() { return (*m_ptr); }
	auto operator*() { return (*m_ptr); }

	IntrusiveScopedPtr& operator=(nullptr_t) noexcept
	{
		this->reset();
		return (*this);
	}

	IntrusiveScopedPtr& operator=(const IntrusiveScopedPtr&) noexcept = delete;
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
#if (defined _DEBUG || defined DEBUG)
		++copiesFromThis;
#endif

		return _Myty(ExplicitCopy());
	}

	// Return copy as unique pointer
	std::unique_ptr<_Ty> get_unique()
	{
		return std::unique_ptr<_Ty>{ ExplicitCopy() };
	}

	void reset(_Ty* ptr) noexcept { m_ptr.reset(ptr); }

#if (defined _DEBUG || defined DEBUG)
	// Deep copy counters, debug only
	bool has_copies() const noexcept { return copiesFromThis > 0; }
	int count_copies() const noexcept { return copiesFromThis; }
#endif
};

} // namespace CoilCl

namespace Cry
{

using namespace CoilCl;

template<typename _Ty, typename... _ArgsTy, typename = typename std::enable_if<!std::is_array<_Ty>::value>::type>
inline IntrusiveScopedPtr<_Ty> MakeIntrusiveScoped(_ArgsTy&&... _Args)
{
	return (IntrusiveScopedPtr<_Ty>(new _Ty{ std::forward<_ArgsTy>(_Args)... }));
}

} // namespace Cry
