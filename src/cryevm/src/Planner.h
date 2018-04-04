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

#ifndef EXIT_SUCCESS
# define EXIT_SUCCESS 0
# define EXIT_FAILURE 1
#endif

using ProgramPtr = Detail::UniquePreservePtr<CoilCl::Program>;

namespace EVM
{

class Strategy;

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
	std::unique_ptr<Strategy> DetermineStrategy();

	const ProgramPtr& Program() const noexcept
	{
		return m_program;
	}

private:
	ProgramPtr m_program{ nullptr };
	Plan m_opt;
};

class Strategy
{
	Planner& m_planner;

protected:
	const ProgramPtr& Program() const noexcept
	{
		return m_planner.Program();
	}

public:
	using ReturnCode = int;

	Strategy(Planner& planner)
		: m_planner{ planner }
	{
	}

	// Check if strategy can run the program
	virtual bool IsRunnable() const noexcept = 0;
	// Run the program with current strategy
	virtual ReturnCode Execute() = 0;
};

} // namespace EVM
