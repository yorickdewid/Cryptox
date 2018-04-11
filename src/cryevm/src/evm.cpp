// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "evm.h"
#include "Planner.h"
#include "UniquePreservePtr.h"

#include "../../coilcl/src/Program.h"

#include <memory>
#include <iostream> // only for cerr

//TODO:
// - Involve planner and determine strategy
//    - Check if local arch compile is required
// - Run program in runner resulting from planner
//    - Either Interpreter
//    - Or Virtual machine
//    - Or native

using ProgramPtr = Detail::UniquePreservePtr<CoilCl::Program>;

// Execute program
EVMAPI int ExecuteProgram(runtime_settings_t *runtime) noexcept
{
	// Capture program pointer and cast into program structure
	ProgramPtr program = ProgramPtr{ runtime->program.program_ptr };

	// Determine strategy for program
	auto runner = EVM::Planner{ std::move(program), EVM::Planner::Plan::ALL }.DetermineStrategy();
	if (!runner->IsRunnable()) {
		return RETURN_NOT_RUNNABLE; //TODO?
	}

	try {
		// Execute the program in the designated strategy
		runtime->return_code = runner->Execute();
	}
	// Catch any runtime errors
	catch (const std::exception& e) {
		//FIXME:
		std::cerr << e.what() << std::endl;
	}

	return RETURN_OK;
}
