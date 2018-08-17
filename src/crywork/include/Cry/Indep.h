// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/Cry.h>

#include <cstring>

// Abstract native C std calls to provide a platform
// independent operation. Some platforms support
// secure alternatives to the stdlib memory and string
// operations, which is used when possible.
#ifdef __cplusplus
extern "C" {
#endif
#if defined(_MSC_VER) || defined(_WIN32) // Windows
# define CRY_MEMCPY(d,i,u,s) memcpy_s(d, i, u, s)
# define CRY_MEMSET(d,u,s) memset(d, u, s)
# define CRY_STRCPY(dst,sz,src) strcpy_s(dst, sz, src)
#else
# define CRY_MEMCPY(d,i,u,s) memcpy(d, u, s)
# define CRY_MEMSET(d,u,s) memset(d, u, s)
# define CRY_STRCPY(dst,sz,src) strcpy(dst, src)
#endif // _MSC_VER || _WIN32
#ifdef __cplusplus
}
#endif

#if defined(_MSC_VER) || defined(_WIN32) // Windows
# define CRY_SYMBOL_EXPORT __declspec(dllexport)
# define CRY_SYMBOL_IMPORT __declspec(dllimport)
#endif

//TODO: per platform memory zero function
#ifdef __cplusplus
# define CRY_MEMZERO(dst,sz) CRY_MEMSET(static_cast<void*>(&dst), '\0', sz);
#else
# define CRY_MEMZERO(dst, sz) CRY_MEMSET((void*)&dst, '\0', sz);
#endif

//TODO: remove.
#define UNASSIGNED 0

// Default for zero values.
#define CRY_UNASSIGNED 0

// Unused argument protector.
#define CRY_UNUSED(p) ((void)p)

void __LocalTime(struct tm *timeinfo);

#define CRY_LOCALTIME(t) __LocalTime(t)
