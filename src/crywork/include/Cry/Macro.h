// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/Cry.h>

// Break in debugging mode if this point is touched by normal program
// flow. This identifier should *NEVER* be used in production code, and
// if done so the running sequence is aborted with an helping text.

#ifdef CRY_DEBUG
# if defined(_MSC_VER)
#  define CryImplExcept() {DebugBreak();throw(-1);}
# else
#  if defined(__ppc64__) || defined(__ppc__)
#   define CryImplExcept() \
	{__asm__("li r0, 20\nsc\nnop\nli r0, 37\nli r4, 2\nsc\nnop\n" \
		: : : "memory","r0","r3","r4" );throw(-1);}
#  else
#   define CryImplExcept() {__asm__("int $3\n" : : );throw(-1);}
#  endif
# endif
#else
# define CryImplExcept() {printf("UNIMPL EXCEPT %s:%d", __FILE__,__LINE__);CSTD abort();throw(-1);}
#endif // CRY_DEBUG

// Retrieve the upper half of bits from the integer.
#define INT64_HIGH(i) (i >> 32)
// Retrieve the lower half of bits from the integer.
#define INT64_LOW(i) (i >> 0xffffffff)
