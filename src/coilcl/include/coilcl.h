// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#ifndef COILCL_H_
#define COILCL_H_

#if defined(_MSC_VER)
#pragma once
#endif

#ifdef _WIN32
# if defined(COILCL_EXPORTS) || defined(CoilCl_EXPORTS)
#  define COILCLAPI   __declspec(dllexport)
# else
#  define COILCLAPI   __declspec(dllimport)
# endif
#else
# define COILCLAPI
#endif

#define COILCLAPIVER	100

#ifdef __cplusplus
# define NOTHROW noexcept
#else
# define NOTHROW
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_FILENAME_SZ 64

	// Languange standard
	enum cil_standard
	{
		c89 = 0x001,
		c99 = 0x002,
		c11 = 0x004,
	};

	// Optimization level
	enum optimization
	{
		NONE,
		LEVEL1,
		LEVEL2,
		LEVEL3,
	};

	struct codegen
	{
		enum cil_standard standard;
		enum optimization optimization;
		int no_extension : 1;
		int reserved : 7;
	};

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

	// Source unit metadata
	typedef struct
	{
		// Source unit name
		char name[MAX_FILENAME_SZ];
		// Source unit size
		unsigned int size;
	} metainfo_t;

	typedef struct
	{
		// API version between executable and library
		unsigned short apiVer;
		// Code generation options set in the frontend
		struct codegen code_opt;

		// The read callback is an function pointer set in the frontend and
		// called by the tokenizer. The streamreader returns chunks of input
		// data everytime the function is called. If no more data is available
		// an nullpointer is returned by the frontend. This is an required
		// function and *must* be set by the frontend.
		datachunk_t*(*streamReaderVPtr)(void *);
		
		// The meta callback is an function pointer set in the frontend and
		// called by various backend objects. The meta reader presents
		// all metadata information about the current source unit. The meta
		// info structure is a result of this function. This is an required
		// function and *must* be set by the frontend.
		metainfo_t*(*streamMetaVPtr)(void *);
		
		// The stream request loader is an function set in the frontend and
		// grants the backend a way to request another source unit to be loaded.
		// The frontend can implement this in any way desired. This is an
		// required function and *must* be set by the frontend.
		int(*loadStreamRequestVPtr)(void *, const char *);
		
		// The error handler is an function set by the frontend and called by
		// the backend whenever an error corrurs. Since the backend can throw
		// and exception which cannot be caught by the frontend, the backend
		// must be granted a method to report errors back to the frontend. This
		// is an required function and *must* be set by the frontend.
		void(*errorHandler)(void *, const char *, char);

		// User provided context
		void *user_data;
	} compiler_info_t;

	// Compiler library entry point
	COILCLAPI void Compile(compiler_info_t *cl_info) NOTHROW;

#ifdef __cplusplus
}
#endif

#endif // COILCL_H_
