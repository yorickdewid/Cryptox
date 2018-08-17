// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/Cry.h>

#if defined(_MSC_VER)
# include <intrin.h>
# include <Windows.h>
#elif defined(BSD)
#include <sys/endian.h>
#else
#include <endian.h>
#endif

// Determine endianness
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

#ifndef __NO_INTERNAL_BO
# ifdef _MSC_VER
#  define BSWAP16(x) ((__int16)_byteswap_ushort(x))
#  define BSWAP32(x) ((__int32)_byteswap_ulong(x))
#  define BSWAP64(x) ((__int64)_byteswap_uint64(x))
# endif // _MSC_VER
# ifdef __GNUC__
#  define BSWAP16(x) ((uint16_t)__builtin_bswap16(x))
#  define BSWAP32(x) ((uint32_t)__builtin_bswap32(x))
#  define BSWAP64(x) ((uint64_t)__builtin_bswap64(x))
# endif // __GNUC__
#endif // __NO_INTERNAL_BO

// Fallback byte swap macros
#if (!defined(BSWAP16) && !defined(BSWAP32) && !defined(BSWAP64)) || defined(__NO_INTERNAL_BO)
# define BSWAP16(x) ( ((x >> 8) | (x << 8)) )
# define BSWAP32(x) ( ((x >> 24) & 0x000000ff) | ((x >>  8) & 0x0000ff00) | \
                      ((x <<  8) & 0x00ff0000) | ((x << 24) & 0xff000000) )
# define BSWAP64(x) ( ((x >> 56) & 0x00000000000000ff) | ((x >> 40) & 0x000000000000ff00) | \
                      ((x >> 24) & 0x0000000000ff0000) | ((x >>  8) & 0x00000000ff000000) | \
                      ((x <<  8) & 0x000000ff00000000) | ((x << 24) & 0x0000ff0000000000) | \
                      ((x << 40) & 0x00ff000000000000) | ((x << 56) & 0xff00000000000000) )
#endif

// Runtime check if arch is big endian
constexpr bool IsBigEndian()
{
	int n = 0x01020304;
	char *p = (char *)&n;
	return (p[0] == 1);
}

// Runtime check if arch is little endian
constexpr bool IsLittleEndian()
{
	int n = 0x1;
	char *p = (char *)&n;
	return (p[0] == 1);
}
