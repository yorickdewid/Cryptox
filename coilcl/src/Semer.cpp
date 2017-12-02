#include "AST.h"
#include "ASTNode.h"
#include "Semer.h"

//TODO: ?
template<typename... _Ty, class _Decl = DeclRefExpr>
inline std::shared_ptr<_Decl> make_ref(_Ty&&... _Args)
{
	return std::shared_ptr<_Decl>{new _Decl{ _Args... }};
}

template<typename _Ty>
inline auto FindTreeType(AST::AST& ast) -> AST::AST::const_iterator
{
	return std::find_if(ast.cbegin(), ast.cend(), [](ASTNode& item)
	{
		return typeid(item) == typeid(_Ty);
	});
}

//XXX: for now
class NotImplementedException : public std::runtime_error
{
public:
	NotImplementedException(const std::string& message) noexcept
		: std::runtime_error{ message }
	{
	}

	explicit NotImplementedException(char const* const message) noexcept
		: std::runtime_error{ message }
	{
	}

	virtual const char *what() const noexcept
	{
		return std::runtime_error::what();
	}
};

CoilCl::Semer::Semer(std::shared_ptr<CoilCl::Profile>& profile, AST::AST&& ast)
	: Stage{ this }
	, m_profile{ profile }
	, m_ast{ std::move(ast) }
{
	//m_resolvStash.Enlist()
}

CoilCl::Semer& CoilCl::Semer::CheckCompatibility()
{
	return (*this);
}

//TODO: loop
// Resolve all static expresions such as
// native type size calculations.
CoilCl::Semer& CoilCl::Semer::StaticResolve()
{
	std::array<std::string, 1> staticLookup{ "sizeof" };

	AST::AST::const_iterator itr = FindTreeType<BuiltinExpr>(m_ast);

	// Found any static expressions
	if (itr != m_ast.cend()) {
		auto builtinExpr = std::dynamic_pointer_cast<BuiltinExpr>(itr.shared_ptr());
		auto declRefName = builtinExpr->FuncDeclRef()->Identifier();

		if (std::any_of(staticLookup.cbegin(), staticLookup.cend(), [&declRefName](const std::string& c) { return c == declRefName; })) {

			// If expression, evaluate outcome
			if (builtinExpr->Expression()) {
				throw NotImplementedException{ "Expression" };
			}
			// No expression, use typename
			else {
				AST::TypeFacade type = builtinExpr->TypeName();

				//TODO: helper ?
				// Replace static builtin operation with integer result
				auto m_data = std::make_unique<CoilCl::Valuedef::ValueObject<int>>(CoilCl::Typedef::BuiltinType{ CoilCl::Typedef::BuiltinType::Specifier::INT }, type.Size());
				auto literal = CoilCl::AST::MakeASTNode<IntegerLiteral>(std::move(m_data));

				//TODO: static 1
				// Emplace current object on existing
				if (auto parent = builtinExpr->Parent().lock()) {
					parent->Emplace(1, literal);
				}
			}
		}
	}

	return (*this);
}

// Run all semantic checks that defines the language,
// this comprises type checking, object scope validation,
// implicit casting and identifier resolving.
CoilCl::Semer& CoilCl::Semer::PreliminaryAssert()
{
	/*AST::AST::const_iterator itr = FindTreeType<DeclRefExpr>(m_ast);

	if (itr != m_ast.cend()) {
		auto declExpr = std::dynamic_pointer_cast<DeclRefExpr>(itr.shared_ptr());
		if (!declExpr->IsResolved()) {
			auto resvName = declExpr->Identifier();
			if (std::any_of(staticLookup.cbegin(), staticLookup.cend(), [&resvName](const std::string& c) { return c == resvName; })) {
				printf("lolz");
			}
		}
	}*/


	//TODO: identifier resolving & scoping
	//TODO: match function prototype with body
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
