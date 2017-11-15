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
	// Output
	auto& Ast() const
	{
		return m_ast;
	}

	// Input
	void Ast(std::shared_ptr<TranslationUnitDecl>& ast) const
	{
		m_ast = ast;
	}

private:
	StageType m_lastStage;
	mutable std::shared_ptr<TranslationUnitDecl> m_ast;
};

} // namespace CoilCl
