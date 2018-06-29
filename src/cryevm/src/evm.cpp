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

#include <boost/lexical_cast.hpp>

#include <memory>

//#define AUTO_CONVERT 1

//TODO:
// - Involve planner and determine strategy
//    - Check if local arch compile is required
// - Run program in runner resulting from planner
//    - Either Interpreter
//    - Or Virtual machine
//    - Or native

using ProgramPtr = Detail::UniquePreservePtr<CoilCl::Program>;

#ifdef AUTO_CONVERT
bool has_digits(const std::string& s)
{
	return s.find_first_not_of("0123456789") == std::string::npos;
}
#endif

void ReworkArgumentList(ArgumentList& list, runtime_settings_t *runtime)
{
	size_t sz = 0;
	if (!runtime->args) {
		return;
	}

	do {
		// Reconstruct parameters and cast to builtin type if possible
		const datachunk_t *arg = runtime->args[sz++];
		auto str = std::string{ arg->ptr, arg->size };
#ifdef AUTO_CONVERT
		if (has_digits(str)) {
			list.emplace_back(boost::lexical_cast<int>(str));
		}
		else {
			list.push_back(std::move(str));
		}
#else
		list.push_back(std::move(str));
#endif
		if (arg->unmanaged_res) {
			auto *tmpVal = const_cast<char*>(arg->ptr);
			free(static_cast<void*>(tmpVal));
			tmpVal = nullptr;
		}
	} while (runtime->args[sz]->ptr != nullptr);
}

void AssertConfiguration(const struct vm_config *config)
{
	//
}

//TODO: check API version from struct
// Execute program
EVMAPI int ExecuteProgram(runtime_settings_t *runtime) noexcept
{
	AssertConfiguration(&runtime->cfg);

	// Capture program pointer and cast into program structure
	ProgramPtr program = ProgramPtr{ runtime->program.program_ptr };

	// Determine strategy for program
	auto runner = EVM::Planner{ std::move(program), EVM::Planner::Plan::ALL }.DetermineStrategy();
	if (!runner->IsRunnable()) {
		return RETURN_NOT_RUNNABLE;
	}

	try {
		ArgumentList args;
		ReworkArgumentList(args, runtime);

		// Execute the program in the designated strategy
		runtime->return_code = runner->Execute(runner->EntryPoint(runtime->entry_point), args);
	}
	// Catch any runtime errors
	catch (const std::exception& e) {
		runtime->error_handler(runtime->user_data, e.what(), true);
	}

	return RETURN_OK;
}
