// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Exportable.h"
#include "Executable.h"

#include <string>

namespace CryExe
{
namespace Meta
{

// Return the image version as string
std::string ImageVersion(Executable& exec);

// Return the program version as string
std::string ProgramVersion(Executable& exec);

} // namespace Meta
} // namespace CryExe
