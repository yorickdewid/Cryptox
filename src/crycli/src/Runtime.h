// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <string>
#include <vector>

#include <boost/variant.hpp>

class ExecutionEnv;

using ArgumentList = std::vector<std::string>;

class VMContract
{
	// Allow the abstract compiler internal access.
	friend class ExecutionEnv;

	// Run compiler.
	virtual std::pair<int, int> Execute() = 0;
	// Set commandline arguments to the program.
	virtual void CommandLineArgs(const ArgumentList&) = 0;
	// Set symbol as entry point.
	virtual void SetEntryPoint(const char *) = 0;
};

class ExecutionEnv
{
public:
	struct RunResult
	{
		int vmExitCode;
		int programExitCode;
	};

public:
	ExecutionEnv(void *programPtr);
	~ExecutionEnv();

	// Setup the environment and runtime settings.
	ExecutionEnv& Setup();
	// Provide program main entry point.
	ExecutionEnv& EntryPoint(const std::string&);
	// Run the program.
	RunResult ExecuteProgram(const ArgumentList = {});

private:
	VMContract * m_virtualMachine = nullptr;
};
