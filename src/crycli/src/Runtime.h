// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <string>

class ExecutionEnv;

class VMContract
{
	// Allow the abstract compiler internal access.
	friend class ExecutionEnv;

	// Run compiler.
	virtual void Execute() = 0;
	// Run compiler.
	virtual void SetEntryPoint(const char *) = 0;
};

class ExecutionEnv
{
public:
	ExecutionEnv(void *programPtr);
	~ExecutionEnv();

	// Setup the environment and runtime settings.
	ExecutionEnv& Setup();
	// Provide program main entry point.
	ExecutionEnv& EntryPoint(const std::string&);
	// Run the program.
	void ExecuteProgram();

private:
	VMContract * m_virtualMachine = nullptr;
};
