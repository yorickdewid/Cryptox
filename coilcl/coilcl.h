// Copyright (c) 2017 Quenza Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef COILCL_H_
#define COILCL_H_

#if defined(_MSC_VER)
#pragma once
#endif

#ifdef _WIN32
# if defined(COILCL_EXPORTS)
#  define COILCLAPI   __declspec(dllexport)
# else
#  define COILCLAPI   __declspec(dllimport)
# endif
#elif
# define PFBASEAPI
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

	struct interpreter
	{
		//
	};

	typedef struct
	{
		size_t size;
		const char *ptr;
		char unmanaged_res;
	} datachunk_t;

	typedef struct
	{
		struct codegen code_opt;
		struct interpreter run_opt;

		// Callback functions serving data exchange between callee and calleer
		datachunk_t*(*streamReaderVPtr)(void*);
		void*(*streamMetaVPtr)();
		int(*loadStreamRequestVPtr)(const char *);

		// User provided context
		void *user_data;
	} compiler_info_t;

	// Compiler library entry point
	//COILCLAPI void Compile(const char* const content);
	COILCLAPI void Compile(compiler_info_t *cl_info);

#ifdef __cplusplus
}
#endif

#endif // COILCL_H_
