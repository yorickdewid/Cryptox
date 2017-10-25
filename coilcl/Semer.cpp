#include "AST.h"
#include "Semer.h"

template<typename... _Ty, class _Decl = DeclRefExpr>
inline std::shared_ptr<_Decl> make_ref(_Ty&&... _Args)
{
	return std::shared_ptr<_Decl>{new _Decl{ _Args... }};
}

CoilCl::Semer::Semer(std::shared_ptr<Compiler::Profile>& profile)
	: m_profile{ profile }
{
}

CoilCl::Semer& CoilCl::Semer::PreliminaryAssert()
{
	return (*this);
}

CoilCl::Semer& CoilCl::Semer::StandardCompliance()
{
	return (*this);
}

void CoilCl::Semer::ResolveIdentifier()
{
	/*const auto& refIdentifier = m_identifierStack.top();
	auto decl = stash->Resolve<VarDecl, Decl>([&refIdentifier](std::shared_ptr<VarDecl>& varPtr) -> bool
	{
	return varPtr->Identifier() == refIdentifier;
	});

	if (decl == nullptr) {
	throw ParseException{ std::string{ "use of undeclared identifier '" + refIdentifier + "'" }.c_str(), 0, 0 };
	}

	m_identifierStack.pop();
	auto ref = make_ref(decl);*/
}
