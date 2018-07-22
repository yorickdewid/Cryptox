// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "ExternalMethod.h"

namespace EVM
{
namespace GlobalExecutionState
{

// Set the external symbol list.
void Set(const std::list<ExternalMethod>&);

// Clear any gobal state.
void UnsetAll();

// Find an external symbol, returns either external method or nullptr.
const ExternalMethod *FindExternalSymbol(const std::string&);

} // namespace GlobalExecutionState
} // namespace EVM
