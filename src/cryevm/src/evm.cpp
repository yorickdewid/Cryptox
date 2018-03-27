// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "evm.h"
#include "Planner.h"

#include "../../coilcl/src/Program.h"

#include <memory>

//TODO:
// - Involve planner and determine strategy
//    - Check if local arch compile is required
// - Run program in runner resulting from planner
//    - Either Interpreter
//    - Or Virtual machine
//    - Or native

using ProgramPtr = std::unique_ptr<CoilCl::Program, void(*)(CoilCl::Program *)>;

namespace Helper {

class Program
{
	// Ingore deletion of object since pointer must be preserved
	static void PreserveObject(CoilCl::Program *ptr)
	{
		CRY_UNUSED(ptr);
	}

public:
	// Capture the raw pointer in a managed object. Since this pointer cannot be freed here, but
	// must be provided to certain methods as a managed object and custom deleter is used. This
	// makes the program a shared scoped pointer.
	static ProgramPtr Capture(program_t *program_raw)
	{
		return ProgramPtr{ reinterpret_cast<CoilCl::Program *>(program_raw), PreserveObject };
	}
};

} // namespace Helper

// Execute program
EVMAPI void Execute(program_t *program_raw) noexcept
{
	// Capture program pointer and cast into program structure
	ProgramPtr program = Helper::Program::Capture(program_raw);

	// Determine strategy for program
	auto runner = EVM::Planner{ std::move(program), EVM::Planner::Plan::ALL }.DetermineStrategy();
	if (!runner.IsRunnable()) {
		throw 1; //TODO
	}

	try {
		// Execute the program in the designated runner
		runner.Execute();
	}
	// Catch any runtime errors
	catch (...) {
		//TODO
	}
}
