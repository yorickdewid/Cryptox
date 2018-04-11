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

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct
	{
		// API version between executable and library
		unsigned short apiVer;
		// Program entry point, this must be a synbol name.
		const char *entryPoint;

		// The read callback is an function pointer set in the frontend and
		// called by the tokenizer. The streamreader returns chunks of input
		// data everytime the function is called. If no more data is available
		// an nullpointer is returned by the frontend. This is an required
		// function and *must* be set by the frontend.
		//datachunk_t*(*streamReaderVPtr)(void *);

		// The meta callback is an function pointer set in the frontend and
		// called by various backend objects. The meta reader presents
		// all metadata information about the current source unit. The meta
		// info structure is a result of this function. This is an required
		// function and *must* be set by the frontend.
		//metainfo_t*(*streamMetaVPtr)(void *);

		// The stream request loader is an function set in the frontend and
		// grants the backend a way to request another source unit to be loaded.
		// The frontend can implement this in any way desired. This is an
		// required function and *must* be set by the frontend.
		//int(*loadStreamRequestVPtr)(void *, const char *);

		// The error handler is an function set by the frontend and called by
		// the backend whenever an error corrurs. Since the backend can throw
		// and exception which cannot be caught by the frontend, the backend
		// must be granted a method to report errors back to the frontend. This
		// is an required function and *must* be set by the frontend.
		void(*errorHandler)(void *, const char *, char);

		// Compiler resulting output. This structure is set by the compiler
		// interface and should be freed by the caller. The structure cannot
		// be used directly, but shall be passed to program compatible components.
		program_t program;
		// User provided context.
		void *user_data;
	} runtime_settings_t;

	// Compiler library entry point
	EVMAPI void Execute(program_t *program) NOTHROW;

#ifdef __cplusplus
}
#endif

#endif // EVM_H_
