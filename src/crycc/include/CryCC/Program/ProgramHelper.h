// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/Program/Program.h>
#include <CryCC/Program/Type.h>

#include <memory>

namespace Util
{

using namespace CryCC::Program;

// Allocate a new program.
template<typename... ArgTypes>
ProgramType MakeProgram(ArgTypes&&... args)
{
    return std::make_unique<CryCC::Program::Program>(std::forward<ArgTypes>(args)...);
}

} // namespace Util
