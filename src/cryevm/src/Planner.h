// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "UniquePreservePtr.h"

#include "../../coilcl/src/Program.h"

using ProgramPtr = Detail::UniquePreservePtr<CoilCl::Program>;

namespace EVM
{

struct Strategy
{
	// Check if strategy can run the program
	bool IsRunnable() const noexcept { return true; }
	// Run the program with current strategy
	void Execute() {}
};

class Planner
{
public:
	enum Plan
	{
		NOARCH_ONLY,
		NATIVE_ONLY,
		ALL,
	};

public:
	Planner(ProgramPtr&& program, Plan plan);
	// Match possible strategies with program
	Strategy DetermineStrategy();

private:
	ProgramPtr m_program{nullptr};
	Plan m_opt;
};

} // namespace EVM
