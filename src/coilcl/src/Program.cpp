// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Program.h"

#include <iostream>

void CoilCl::Program::ConditionTracker::Advance()
{
	//TODO
}

void CoilCl::Program::PrintSymbols()
{
	for (const auto& node : m_symbols) {
		std::cout << "Symbol: " << node.first << std::endl;
	}
}

//void CoilCl::Program::Bind(std::unique_ptr<CoilCl::Program>&& program, std::shared_ptr<AST::AST>&& ast)
//{
//	program = std::make_unique<CoilCl::Program>(std::move(*(program.release())), std::move(ast));
//}
