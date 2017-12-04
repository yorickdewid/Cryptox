// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "AST.h"
#include "Stage.h"

#include <map>
#include <vector>
#include <memory>

namespace CoilCl
{

// Program is the resulting structure for all compiler stages.
// Only stages are allowed to alter the program internals. The
// program is passed to a program runner in which case the program
// is run, or the program structure is dissected in an analyzer.
class Program final
{
public:
	enum Condition
	{
		CANONICAL = 1,
		FUNDAMENTAL,
		STATIC_RESOLVED,
		ASSERTION_PASSED,
		COMPLANT,
		OPTIMIZED,
		STRIPPED,
	};

public:
	// Program constructors
	Program() = default;
	Program(const Program&) = delete;
	Program(Program&&) = default;
	Program(Program&& other, std::shared_ptr<TranslationUnitDecl>&& ast)
		: m_ast{ new AST::AST{ std::move(ast) } }
		, m_treeCondition{ other.m_treeCondition }
		, m_lastStage{ other.m_lastStage }
	{
	}

	// Do not assign
	Program& operator=(const Program&) = delete;
	Program& operator=(Program&&) = delete;

	// AST operations
	inline auto Ast() { return m_ast->tree_ref(); }
	inline auto AstPassthrough() const { return m_ast->operator->(); }

	// Symbol operations
	inline bool HasSymbol(const std::string& name) const { return m_symbols.find(name) != m_symbols.end(); }

	// Check program status
	inline auto IsRunnable() const { return m_treeCondition >= Condition::ASSERTION_PASSED; }
	inline auto IsLanguage() const { return m_treeCondition >= Condition::COMPLANT; }
	inline auto IsOptimized() const { return m_treeCondition >= Condition::OPTIMIZED; }

private:
	Condition m_treeCondition;
	StageType m_lastStage;

private:
	std::map<std::string, std::weak_ptr<ASTNode>> m_symbols;
	std::unique_ptr<AST::AST> m_ast;
};

} // namespace CoilCl
