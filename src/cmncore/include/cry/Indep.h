// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Cry.h"

#include <cstring>

// Abstract native C std calls to provide a platform
// independent operation. Some platforms support
// secure alternatives to the stdlib memory and string
// operations, which is used when possible.
#if defined(_MSC_VER) || defined(_WIN32) // Windows
#  define CRY_MEMCPY(d,i,u,s) memcpy_s(d, i, u, s)
#  define CRY_MEMSET(d,u,s) memset(d, u, s)
#  define CRY_STRCPY(dst,sz,src) strcpy_s(dst, sz, src)
#else
#  define CRY_MEMCPY(d,i,u,s) memcpy(d, u, s)
#  define CRY_MEMSET(d,u,s) memset(d, u, s)
#  define CRY_STRCPY(dst,sz,src) strcpy(dst, src)
#endif // _MSC_VER || _WIN32

//TODO: per platform memory zero function
#define CRY_MEMZERO(dst,sz) CRY_MEMSET(static_cast<void*>(&dst), '\0', sz);

#define UNASSIGNED 0

#define CRY_UNUSED(p) ((void)p)

#ifdef _MSC_VER // Visual Studio
#  define INTERSUB
#else
#  if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) // C99
#    if defined(__GNUC__) || defined(__clang__)
#      define INTERSUB
#    else
#      define INTERSUB
#    endif
#  else
#    define INTERSUB
#  endif // __STDC_VERSION__
#endif // _MSC_VER
