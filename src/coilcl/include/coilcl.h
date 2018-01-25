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
	
	enum cil_standard
	{
		c89 = 0x001,
		c99 = 0x002,
		c11 = 0x004,
	};

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
	};

	// C compatible string structure
	typedef struct
	{
		unsigned int size;
		const char *ptr;
		char unmanaged_res;
	} datachunk_t;

	// Source unit metadata
	typedef struct
	{
		char name[64];
	} metainfo_t;

	typedef struct
	{
		unsigned short apiVer;

		struct codegen code_opt;

		// Callback functions serving data exchange between callee and calleer
		datachunk_t*(*streamReaderVPtr)(void *);
		metainfo_t*(*streamMetaVPtr)(void *);
		int(*loadStreamRequestVPtr)(void *, const char *);
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
