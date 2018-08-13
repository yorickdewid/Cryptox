// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/Program/Program.h>

#include <iostream>

#ifdef CRY_DEBUG
# define CRY_DEBUG_TRACE CRY_DEBUG_TRACE_ALL || 1
#endif

namespace CryCC
{
namespace Program
{

Program::Program(AST::AST&& tree)
	: m_ast{ new AST::AST{ std::move(tree) } }
{
}

Program::Program(Program&& other, AST::ASTNodeType&& ast)
	: m_ast{ new AST::AST{ std::move(ast) } }
	, m_treeCondition{ other.m_treeCondition }
	, m_lastStage{ other.m_lastStage }
	, m_locked{ other.m_locked }
{
}

} // namespace Program
} // namespace CryCC
