#pragma once

#include "Profile.h"
#include "Stage.h"

namespace CoilCl
{

class Semer : public Stage
{
public:
	Semer(std::shared_ptr<Compiler::Profile>& profile);
	
	template<typename _Ty>
	Semer& Syntax(_Ty&& ast)
	{
		m_ast = std::move(ast);
		return (*this);
	}

	Semer& StaticResolve();
	Semer& PreliminaryAssert();
	Semer& StandardCompliance();

	void ResolveIdentifier();

private:
	std::shared_ptr<ASTNode> m_ast;
	std::shared_ptr<Compiler::Profile> m_profile;
};

} // namespace CoilCl
