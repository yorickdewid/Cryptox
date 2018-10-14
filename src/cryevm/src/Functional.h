// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryEVM/ExternalMethod.h>
#include <CryEVM/ExternalValue.h>

#include <list>

//FUTURE: Forward decl. for SymbolIndex

namespace EVM
{

using ExternalMethodRegistry = std::list<ExternalMethod>;

ExternalMethodRegistry SymbolIndex();

} // namespace EVM
