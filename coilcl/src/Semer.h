#pragma once

#include "Profile.h"
#include "Stage.h"

namespace CoilCl
{

class Semer : public Stage<Semer>
{
public:
	Semer(std::shared_ptr<CoilCl::Profile>& profile, AST::AST&& ast);
	
	std::string Name() const { return "Semer"; }

	Semer& CheckCompatibility();
	Semer& StaticResolve();
	Semer& PreliminaryAssert();
	Semer& StandardCompliance();

	void ResolveIdentifier();

private:
	AST::AST m_ast;
	std::shared_ptr<CoilCl::Profile> m_profile;
};

} // namespace CoilCl
