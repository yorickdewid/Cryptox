// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/Cry.h>

#ifdef _WIN32
# define CRY_WINDOWS 1
# ifdef _WIN64
#  define CRY_WINDOWS64 1
# else
# define CRY_WINDOWS32 1
#endif
#elif __APPLE__
# include "TargetConditionals.h"
# if TARGET_IPHONE_SIMULATOR
#  define CRY_IOS_SIM 1
# elif TARGET_OS_IPHONE
#  define CRY_IOS 1
# elif TARGET_OS_MAC
#  define CRY_OSX 1
# else
#  error "Unknown Apple platform"
# endif
#elif __linux__
# define CRY_LINUX 1
#elif __unix__
# define CRY_UNIX 1
#elif defined(_POSIX_VERSION)
# define CRY_POSIX 1
#else
# error "Unknown OS"
#endif
