// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#ifndef EVM_H_
#define EVM_H_

#include <coilcl.h>

#ifdef _MSC_VER
#pragma once
#endif

#ifdef _WIN32
# if defined(EVM_EXPORTS) || defined(CryEVM_EXPORTS)
#  define EVMAPI   __declspec(dllexport)
# else
#  define EVMAPI   __declspec(dllimport)
# endif
#else
# define EVMAPI
#endif

#define EVMAPIVER	100

#ifdef __cplusplus
# define NOTHROW noexcept
#else
# define NOTHROW
#endif

#ifdef __cplusplus
extern "C" {
#endif

	// Compiler library entry point
	EVMAPI void Execute(program_t *program) NOTHROW;

#ifdef __cplusplus
}
#endif

#endif // EVM_H_
