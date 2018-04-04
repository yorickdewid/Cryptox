// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Interpreter.h"

using namespace EVM;

Interpreter::Interpreter(Planner& planner)
	: Strategy{ planner }
{
	//
}

Interpreter::~Interpreter()
{
	//
}

// Check if strategy can run the program
bool Interpreter::IsRunnable() const noexcept
{
	return Program()->Condition().IsRunnable() || true;
}

// Run the program with current strategy
Interpreter::ReturnCode Interpreter::Execute()
{
	Program()->Ast();
	return EXIT_SUCCESS;
}
