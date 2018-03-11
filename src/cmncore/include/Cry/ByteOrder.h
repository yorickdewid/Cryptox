// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Cry.h"

#if defined(_MSC_VER)
# include <intrin.h>
# include <Windows.h>
#elif defined(BSD)
#include <sys/endian.h>
#else
#include <endian.h>
#endif

#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
    defined(REG_DWORD) && REG_DWORD == REG_DWORD_BIG_ENDIAN || \
    defined(__BIG_ENDIAN__) || \
    defined(__ARMEB__) || \
    defined(__THUMBEB__) || \
    defined(__AARCH64EB__) || \
    defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__)
# define CRY_BIG_ENDIAN 1
#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
    defined(REG_DWORD) && REG_DWORD == REG_DWORD_LITTLE_ENDIAN || \
    defined(__LITTLE_ENDIAN__) || \
    defined(__ARMEL__) || \
    defined(__THUMBEL__) || \
    defined(__AARCH64EL__) || \
    defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)
# define CRY_LITTLE_ENDIAN 1
#else
# error "Unknown architecture"
#endif

#ifdef _MSC_VER
# define BSWAP16(x) ((unsigned short)_byteswap_ushort(x))
# define BSWAP32(x) ((unsigned int)_byteswap_ulong(x))
# define BSWAP64(x) ((__int64)_byteswap_uint64(x))
#endif // _MSC_VER
#ifdef __GNUC__
# define BSWAP16(x) ((uint16_t)__builtin_bswap16(x))
# define BSWAP32(x) ((uint32_t)__builtin_bswap32(x))
# define BSWAP64(x) ((uint64_t)__builtin_bswap64(x))
#endif // __GNUC__

#if !defined(BSWAP16) && !defined(BSWAP32) && !defined(BSWAP64)
# define BSWAP16(x) ((x >> 8) | (x << 8))
# define BSWAP32(x) ((x >> 24) & 0xff) \
                    | ((x << 8) & 0xff0000) \
                    | ((x >> 8) & 0xff00) \
                    | ((x << 24) & 0xff000000)
# define BSWAP64(x) ((x & 0x00000000ffffffff) << 32) | ((x & 0xffffffff00000000) >> 32) \
                    | ((x & 0x0000ffff0000ffff) << 16) | ((x & 0xffff0000ffff0000) >> 16) \
                    | ((x & 0x00ff00ff00ff00ff) << 8) | ((x & 0xff00ff00ff00ff00) >> 8)
#endif

// Runtime check if arch is little endian
constexpr bool IsLittleEndian()
{
	short int n = 0x1;
	char *p = (char *)&n;
	return (p[0] == 1);
}
