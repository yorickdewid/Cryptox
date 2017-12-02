#pragma once

#include "AST.h"
#include "ASTNode.h"
#include "Stage.h"

#include <memory>
#include <vector>

namespace CoilCl
{

// Program is the resulting structure for all compiler stages.
// Only stages are allowed to alter the program internals.
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
	Program() = default;
	Program(const Program&) = delete;

	Program(Program&& other, std::shared_ptr<TranslationUnitDecl>&& ast)
		: m_ast{ new AST::AST{ std::move(ast) } }
		, m_treeCondition{ other.m_treeCondition }
		, m_lastStage{ other.m_lastStage }
	{
	}

	inline auto Ast() { return m_ast->tree_ref(); }

	inline auto AstPassthrough() const { return m_ast->operator->(); }

	// Syntax becomes program after assertion passed
	inline auto IsRunnable() const { return m_treeCondition >= Condition::ASSERTION_PASSED; }

	// Program follows a language if it meets the language specification
	inline auto IsLanguage() const { return m_treeCondition >= Condition::COMPLANT; }

private:
	Condition m_treeCondition;
	StageType m_lastStage;
	std::unique_ptr<AST::AST> m_ast;
};

} // namespace CoilCl
