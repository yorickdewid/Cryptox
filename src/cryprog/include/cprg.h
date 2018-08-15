// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#ifndef CPRG_H_
#define CPRG_H_

#if defined(_MSC_VER)
#pragma once
#endif

#include <common.h>
#include <program.h>

#ifdef _WIN32
# if defined(CRYPROG_EXPORTS) || defined(CryProg_EXPORTS)
#  define CPRGAPI   __declspec(dllexport)
# else
#  define CPRGAPI   __declspec(dllimport)
# endif
#else
# define CPRGAPI
#endif

#define CPRGAPIVER	100

#ifdef __cplusplus
extern "C" {
#endif

	// Release program object.
	CPRGAPI void ReleaseProgram(program_t *) NOTHROW;

	// Source unit metadata.
	typedef struct
	{
		// Compiler resulting output. This structure is set by the compiler
		// interface and should be freed by the caller. The structure cannot
		// be used directly, but shall be passed to program compatible components.
		program_t program;

		// Check if the program is healty. A non zero result indicates the program
		// passed basic assertions and is runnable.
		int is_healthy;

		// A non zero value indicates if the program is locked.
		int is_locked;

		// Number of function symbols. Zero means program contains no symbols. This
		// check is important for futher program processing.
		int symbols;

		// Number of resultsets. A resultset is the product of a program translation
		// into another data format.
		int result_sets;

		// Last program phase recorded by a stage. This essentially serves as a
		// checkpoint in case the program experiences an anomaly.
		unsigned int last_phase;

		// Last compiler stage processing the program.
		unsigned int last_stage;
	} program_info_t;

	// Get program information.
	CPRGAPI void ProgramInfo(program_info_t *) NOTHROW;

	// Library version information.
	CPRGAPI void GetLibraryInfo(library_info_t *) NOTHROW;

	// C function defines.
#define cprg_free_program(p) ReleaseProgram(p)
#define cprg_program_info(p) ProgramInfo(p)
#define cprg_get_library_info(p) GetLibraryInfo(p)

#ifdef __cplusplus
}
#endif

#endif // CPRG_H_
