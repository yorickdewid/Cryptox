// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Typedef.h"

namespace CoilCl
{
namespace Conv
{

template<typename _Ty1, typename Ty2>
struct is_convertible : std::false_type { };

template<typename _Ty1>
struct is_convertible<_Ty1, _Ty1> : std::true_type {};

template<>
struct is_convertible<Typedef::RecordType, Typedef::RecordType> : std::false_type {};

template<>
struct is_convertible<Typedef::VariadicType, Typedef::VariadicType> : std::false_type {};

template<typename _InputTy, typename _OutputTy>
class Cast
{

};

} // namespace Conv
} // namespace CoilCl
