#pragma once

#include "AST.h"
#include "Stage.h"

#include <memory>
#include <vector>

namespace CoilCl
{

// Program is the resulting structure for all compiler stages.
// Only stages are allowed to alter the program internals.
class Program
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
		: m_ast{ std::move(ast) }
		, m_treeCondition{ other.m_treeCondition }
		, m_lastStage{ other.m_lastStage }
	{
	}

	//inline void Ast(std::shared_ptr<TranslationUnitDecl>& ast) const { m_ast = ast; }
	inline auto& Ast() const { return m_ast; }

	// Syntax becomes program after assertion passed
	inline auto IsRunnable() const { return m_treeCondition >= Condition::ASSERTION_PASSED; }

	// Program follows a language if it meets the language specification
	inline auto IsLanguage() const { return m_treeCondition >= Condition::COMPLANT; }

private:
	Condition m_treeCondition;
	StageType m_lastStage;
	mutable std::shared_ptr<TranslationUnitDecl> m_ast;
};

} // namespace CoilCl
