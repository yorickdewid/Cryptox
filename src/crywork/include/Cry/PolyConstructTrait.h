// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Cry.h"

namespace Cry
{

// Polyconstruct contract demands the implementing class
// to provide creation and cloning methods so that if the
// class is abstract and polymorphic, the callee can still
// request copies of the object.
struct PolyConstruct
{
	virtual PolyConstruct *Construct() const = 0;
	virtual PolyConstruct *Copy() const = 0;
	virtual ~PolyConstruct() {}
};

template<typename Type>
struct IsPolyconstructable : std::is_base_of<PolyConstruct, Type>
{
};

template<typename Type>
constexpr bool IsPolyconstructableV = IsPolyconstructable<Type>::value;

} // namespace Cry
