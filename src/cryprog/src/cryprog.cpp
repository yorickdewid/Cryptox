// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

// Local includes.
#include "cprg.h"

// Project includes.
#include <CryCC/Program.h>

// Framework includes.
#include <EventLog.h>
#include <Cry/Cry.h>
#include <Cry/Config.h>

// Language includes.
#include <string>
#include <iostream>
#include <functional>

#ifdef CRY_DEBUG
# define CRY_DEBUG_TRACE CRY_DEBUG_TRACE_ALL || 1
//# define CRY_DEBUG_TESTING 1
#endif

// [ API ENTRY ]
// Release program resources.
CPRGAPI void ReleaseProgram(program_t *program) NOTHROW
{
	assert(program);
	if (program->program_ptr) {
		delete static_cast<CryCC::Program::Program *>(program->program_ptr);
		program->program_ptr = nullptr;
	}
}

// [ API ENTRY ]
// Retrieve status and health information from program structure. The
// returning result is pushed into a stateless strucutre.
CPRGAPI void ProgramInfo(program_info_t *program_info) NOTHROW
{
	assert(program_info);

	// Early return if program pointer is empty.
	if (!program_info->program.program_ptr) {
		program_info->is_healthy = false;
		return;
	}

	CryCC::Program::Program *program = static_cast<CryCC::Program::Program *>(program_info->program.program_ptr);
	program_info->is_healthy = program->HasSymbols() && program->operator bool() && program->IsLocked();
	program_info->is_locked = program->IsLocked();
	program_info->symbols = program->SymbolCount();
	program_info->result_sets = false;
	program_info->last_phase = false;
	program_info->last_stage = false;
}

// [ API ENTRY ]
// Get library information.
CPRGAPI void GetLibraryInfo(library_info_t *info) NOTHROW
{
	info->version_number.major = PRODUCT_VERSION_MAJOR;
	info->version_number.minor = PRODUCT_VERSION_MINOR;
	info->version_number.patch = PRODUCT_VERSION_PATCH;
	info->version_number.local = PRODUCT_VERSION_LOCAL;
	info->product = PROGRAM_NAME;
	info->api_version = CPRGAPIVER;
	info->description = PROGRAM_DESCRIPTION;
}
