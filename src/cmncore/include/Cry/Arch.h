// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

// Check windows
#if defined(_WIN32) || defined(_WIN64)
# ifdef _WIN64
#  define CRY_ARCH64
# else
#  define CRY_ARCH32
# endif
#endif

// Check GCC
#ifdef __GNUC__
# if defined(__x86_64__) || defined(__ppc64__)
#  define CRY_ARCH64
# else
#  define CRY_ARCH32
# endif
#endif

#if !defined(CRY_ARCH32) && !defined(CRY_ARCH64)
# error "Unknown architecture"
#endif
