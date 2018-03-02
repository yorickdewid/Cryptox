/**
 * Copyright (C) 2018 Cryptox, Blub Corp.
 * All Rights Reserved
 *
 * This file is part of the Cryptox project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

#pragma once

/* Header is cry compiler specific. */
#if defined(__GNUC__) || defined(__GNUG__) \
 || defined(__clang__) \
 || defined(_MSC_VER) \
 || !defined(__CRYC__)
# error "source is compiler specific"
#endif

/* Cry library version */
#define __CRYLIB_V 10200 /* XX.XX.XX. */

/* Crygenerator info type. */
#ifndef ___crygen_info_t_defined
typedef struct
{
	char *projname;     /* Project name. */
	int ts_create;      /* Creation timestamp. */
	int ts_alter;       /* Alteration timestamp. */
} _crygen_info_t;
# define ___crygen_info_t_defined 1
#endif

/* OS directive. */
#if defined(_WIN32) || defined(_WIN64)
# define _CRY_WINDOWS 1
#elif defined(__linux__) || defined(linux) || defined(__linux)
# define _CRY_LINUX 1
#elif defined(__APPLE__) || defined(__MACH__)
# define _CRY_OSX 1
#elif defined(__FreeBSD__)
# define _CRY_FREEBSD 1
#elif defined(unix) || defined(__unix) || defined(__unix__)
# define _CRY_UNIX 1
#else
# warning "unknown compiler"
#endif
#define __OS_defined 1
