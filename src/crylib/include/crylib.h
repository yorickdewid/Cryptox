// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#define	_CRYLIB_H

// Header is cry compiler specific. Throw a compiler error if this
// is not a supported compiler. This list can be  to add
// unsupported compilers.
#if defined(__GNUC__) || defined(__GNUG__) \
 || defined(__clang__) \
 || defined(_MSC_VER) \
 || !defined(__CRYC__)
# error "module is compiler specific"
#endif

// Cry library version.
#define __CRYLIB_V 10200 //FORMAT: X.XX.XX.
#define __CRYLIB_V_Q1 01
#define __CRYLIB_V_Q2 02
#define __CRYLIB_V_Q3 00

// Crygenerator info type.
#ifndef ___crygen_info_t_defined
typedef struct
{
	char *projname;     // Project name.
	int ts_create;      // Creation timestamp.
	int ts_alter;       // Alteration timestamp.
} _crygen_info_t;
# define ___crygen_info_t_defined
#endif

// OS directive.
#if defined(_WIN32) || defined(_WIN64)
# define _CRY_WINDOWS
#elif defined(__linux__) || defined(linux) || defined(__linux)
# define _CRY_LINUX
#elif defined(__APPLE__) || defined(__MACH__)
# define _CRY_OSX
#elif defined(__FreeBSD__)
# define _CRY_FREEBSD
#elif defined(unix) || defined(__unix) || defined(__unix__)
# define _CRY_UNIX
#else
# warning "unknown compiler"
#endif
#define __OS_defined

#define __USE_ALL 11

// Module option switches.
#ifdef __USE_ALL
# define __USE_ISOC99
# define __USE_ISOC11
# define __USE_ISOC17
# define __USE_CRYC
# define __USE_MISC
# define __USE_XOPEN
# define __USE_XOPEN_EXTENDED
# define __USE_FILE_OFFSET64
# define __USE_LARGEFILE64
#endif

//
// Module extensions.
//

#ifdef __USE_CRYC
// Evaluate the given line as source code.
extern int eval(const char *__source);

// Pause the current execution phase.
extern void pause(void);

// Return the name of the current processing unit.
extern char *current_unit(void);
#endif
