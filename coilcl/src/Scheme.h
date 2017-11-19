#pragma once

#include "AST.h"
#include "Stage.h"

#include <memory>
#include <vector>

namespace CoilCl
{

class Scheme
{
public:
	enum ProgramCondition
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
	inline auto& Ast() const { return m_ast; }
	inline void Ast(std::shared_ptr<TranslationUnitDecl>& ast) const { m_ast = ast; }

	// Syntax becomes program after assertion passed
	inline auto IsRunnable() const { return m_treeCondition >= ProgramCondition::ASSERTION_PASSED; }

	// Program follows a language if it meets the language specification
	inline auto IsLanguage() const { return m_treeCondition >= ProgramCondition::COMPLANT; }

private:
	ProgramCondition m_treeCondition;
	StageType m_lastStage;
	mutable std::shared_ptr<TranslationUnitDecl> m_ast;
};

} // namespace CoilCl
