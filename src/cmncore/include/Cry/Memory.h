// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Cry.h"

#include <memory>

namespace Cry
{

template<typename _ToTy, typename _FromTy>
CSTD unique_ptr<_ToTy> static_unique_pointer_cast(CSTD unique_ptr<_FromTy>&& old)
{
	return CSTD unique_ptr<_ToTy>{ static_cast<_ToTy*>(old.release()) };
}

template<class _Ty1, typename _Ty2>
constexpr _Ty1& side_cast(_Ty2 *_opaquePtr) noexcept
{
	return static_cast<_Ty1&>(*static_cast<_Ty1 *>(const_cast<typename CSTD remove_const<_Ty2>::type*>(_opaquePtr)));
}

} // namespace Cry
