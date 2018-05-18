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

#include <program.h>
#include <data.h>

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
		cil = 10,
		c89 = 89,
		c99 = 99,
		c11 = 11,
		c17 = 17,
	};

	// Optimization level
	enum optimization
	{
		NONE,
		LEVEL1,
		LEVEL2,
		LEVEL3,
	};

	// Code generation options
	struct codegen
	{
		enum cil_standard standard;
		enum optimization optimization;

		// Disable all compiler extensions
		int no_extension : 1;
		// Dump preprocessed source and exit
		int dump_preproc : 1;
		// Dump resulting section to file(s)
		int dump_section : 1;
		// Include comments in program tree
		int keep_comment : 1;
		// Prevent the removal of unused structures
		int keep_zero_ref_cnt : 1;
		
		int reserved : 3;
	};

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
		unsigned short apiVer; //TODO: rename
		// Code generation options set in the frontend
		struct codegen code_opt;

		// The read callback is an function pointer set in the frontend and
		// called by the tokenizer. The streamreader returns chunks of input
		// data everytime the function is called. If no more data is available
		// an nullpointer is returned by the frontend. This is an required
		// function and *must* be set by the frontend.
		datachunk_t*(*streamReaderVPtr)(void *); //TODO: rename

		// The meta callback is an function pointer set in the frontend and
		// called by various backend objects. The meta reader presents
		// all metadata information about the current source unit. The meta
		// info structure is a result of this function. This is an required
		// function and *must* be set by the frontend.
		metainfo_t*(*streamMetaVPtr)(void *); //TODO: rename

		// The stream request loader is an function set in the frontend and
		// grants the backend a way to request another source unit to be loaded.
		// The frontend can implement this in any way desired. This is an
		// required function and *must* be set by the frontend.
		int(*loadStreamRequestVPtr)(void *, const char *); //TODO: rename

		// The error handler is an function set by the frontend and called by
		// the backend whenever an error corrurs. Since the backend can throw
		// and exception which cannot be caught by the frontend, the backend
		// must be granted a method to report errors back to the frontend. This
		// is an required function and *must* be set by the frontend.
		void(*errorHandler)(void *, const char *, int); //TODO: rename and set error code

		// Compiler resulting output. This structure is set by the compiler
		// interface and should be freed by the caller. The structure cannot
		// be used directly, but shall be passed to program compatible components.
		program_t program;
		// User provided context.
		void *user_data;
	} compiler_info_t;

	// Compiler entry point
	COILCLAPI void Compile(compiler_info_t *) NOTHROW;

	// Result section tag
	enum resultsection_tag
	{
		AIIPX,
		CASM,
		NATIVE,
		COMPLEMENTARY,
	};

	// Result inquery
	typedef struct
	{
		// Query result set tag
		enum resultsection_tag tag;
		// Compiler resulting output
		program_t program;
		// Pointer to the requested contents
		datachunk_t content;
	} result_t;

	// Retrieve resulting section from program
	COILCLAPI void GetResultSection(result_t *) NOTHROW;

	typedef struct
	{
		struct
		{
			// Version major part
			short int major;
			// Version minor part
			short int minor;
			// Version patch level
			short int patch;
			// Subversion part
			short int local;
		} version_number;

		// API version number
		short int api_version;
		// Product name
		const char *product;
		// Product description
		const char *description;
	} library_info_t;

	// Library version information
	COILCLAPI void GetLibraryInfo(library_info_t *) NOTHROW;

// C function defines
#define compile(c) Compile(c)
#define get_result_section(r) GetResultSection(r)
#define get_library_info(i) GetLibraryInfo(i)

#ifdef __cplusplus
}
#endif

#endif // COILCL_H_
