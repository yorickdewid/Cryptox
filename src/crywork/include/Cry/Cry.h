// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

//TODO: remove confusing macros
#define CSTD std::
#define CNOEXCEPT noexcept

// Enable debug macro if in debugging mode.
#ifndef CRY_DEBUG
# if defined(DEBUG) || defined(_DEBUG) || !defined(NDEBUG)
#  define CRY_DEBUG 1
# endif
#endif

// On Windows, include some of the Windows API. The Windows API is required
// to include specific Windows macros being abstracted in the Cry Framework.
#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN 1
# include <Windows.h>
#endif

// Remove to disable automaic include of common header files.
#define AUTO_INCLUDE 1

// Include common core files by default. Anything that should be loaded
// throughout the entire Cry Framrwork must be enlisted below.
#ifdef AUTO_INCLUDE
# include "Indep.h"
# include "OS.h"
# include "Arch.h"
# include "Macro.h"
#endif
