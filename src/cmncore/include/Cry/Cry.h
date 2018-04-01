// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#define CSTD std::
#define CNOEXCEPT noexcept

// Remove to disable automaic include of common header files
#define AUTO_INCLUDE 1

// Include common core files by default
#ifdef AUTO_INCLUDE
# include "Indep.h"
# include "OS.h"
#endif
