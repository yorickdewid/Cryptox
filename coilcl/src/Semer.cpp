#include "AST.h"
#include "Semer.h"

template<typename... _Ty, class _Decl = DeclRefExpr>
inline std::shared_ptr<_Decl> make_ref(_Ty&&... _Args)
{
	return std::shared_ptr<_Decl>{new _Decl{ _Args... }};
}

CoilCl::Semer::Semer(std::shared_ptr<Compiler::Profile>& profile)
	: Stage{ this }
	, m_profile{ profile }
{
}

// Resolve all static expresions such as
// native type size calculations.
CoilCl::Semer& CoilCl::Semer::StaticResolve()
{
	//TODO: compile time expression resolving
	return (*this);
}

// Run all semantic checks that defines the language,
// this comprises type checking, object scope validation,
// implicit casting and identifier resolving.
CoilCl::Semer& CoilCl::Semer::PreliminaryAssert()
{
	//TODO: identifier resolving & scoping
	//TODO: type checking
	//TODO: implicit cast
	//TODO: object usage counting
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

	/////////

	/*auto funcProto = stash->Resolve<FunctionDecl>([&func](std::shared_ptr<FunctionDecl>& funcPtr) -> bool
	{
	return funcPtr->Identifier() == func->Identifier()
	&& funcPtr->IsPrototypeDefinition();
	});*/

	// Bind function to prototype
	//if (funcProto) { func->BindPrototype(funcProto); }
}
