// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#ifndef EVM_H_
#define EVM_H_

#ifdef _MSC_VER
#pragma once
#endif

#include <program.h>

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

#define RETURN_OK 0
#define RETURN_NOT_RUNNABLE 1

#ifdef __cplusplus
extern "C" {
#endif

	// C compatible string structure
	typedef struct
	{
		// Size of the data chunk
		unsigned int size;
		// Pointer to data location
		const char *ptr;
		// Boolean indicating this memory block should be freed by the backend
		int unmanaged_res;
	} datachunk_t;

	typedef struct
	{
		// API version between executable and library
		unsigned short apiVer;
		// Program entry point, this must be a synbol name.
		const char *entry_point;
		// Program exit code. Zero indicates exit with success.
		int return_code;

		// The error handler is an function set by the frontend and called by
		// the backend whenever an error corrurs. Since the backend can throw
		// and exception which cannot be caught by the frontend, the backend
		// must be granted a method to report errors back to the frontend. This
		// is an required function and *must* be set by the frontend.
		void(*error_handler)(void *, const char *, int);

		// Compiler resulting output. This structure is set by the compiler
		// interface and should be freed by the caller. The structure cannot
		// be used directly, but shall be passed to program compatible components.
		program_t program;
		// Program runtime arguments.
		const datachunk_t **args;
		// User provided context.
		void *user_data;
	} runtime_settings_t;

	// Compiler library entry point
	EVMAPI int ExecuteProgram(runtime_settings_t *) NOTHROW;

#ifdef __cplusplus
}
#endif

#endif // EVM_H_
