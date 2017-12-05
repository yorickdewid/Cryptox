#include "AST.h"
#include "ASTNode.h"
#include "Semer.h"

//XXX: for now
template<typename _InputIt, typename _UnaryPredicate, typename _UnaryCallback>
void OnMatch(_InputIt first, _InputIt last, _UnaryPredicate p, _UnaryCallback c)
{
	for (; first != last; ++first) {
		if (p(*first)) {
			c(first);
		}
	}
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
}

CoilCl::Semer& CoilCl::Semer::CheckCompatibility()
{
	return (*this);
}

// Resolve all static expresions such as native type size calculations
// and inject the expression result back into the tree
CoilCl::Semer& CoilCl::Semer::StaticResolve()
{
	const std::array<std::string, 1> staticLookup{ "sizeof" };

	AST::ASTEqual<BuiltinExpr> eqOp;
	OnMatch(m_ast.begin(), m_ast.end(), eqOp, [&staticLookup](AST::AST::iterator itr)
	{
		auto builtinExpr = std::dynamic_pointer_cast<BuiltinExpr>(itr.shared_ptr());
		auto declRefName = builtinExpr->FuncDeclRef()->Identifier();

		if (std::any_of(staticLookup.cbegin(), staticLookup.cend(), [&declRefName](const std::string& c) { return c == declRefName; })) {

			// If expression, evaluate outcome
			if (builtinExpr->Expression()) {
				throw NotImplementedException{ "Expression" };
			}
			// No expression, use typename
			else {
				// Replace static builtin operation with integer result
				auto m_data = Util::MakeValueObject<int>(Typedef::BuiltinType::Specifier::INT, builtinExpr->TypeName().Size());
				auto literal = CoilCl::AST::MakeASTNode<IntegerLiteral>(std::move(m_data));

				// Emplace current object on existing
				if (auto parent = builtinExpr->Parent().lock()) {
					auto parentChildren = parent->Children();

					auto selfListItem = std::find_if(parentChildren.begin(), parentChildren.end(), [=](std::weak_ptr<ASTNode>& wPtr)
					{
						return wPtr.lock() == builtinExpr;
					});

					if (selfListItem != parentChildren.end()) {
						size_t idx = std::distance(parentChildren.begin(), selfListItem);
						parent->Emplace(idx, literal);
					}
				}
			}
		}
	});

	return (*this);
}

// Run all semantic checks that defines the language,
// this comprises type checking, object scope validation,
// implicit casting and identifier resolving.
CoilCl::Semer& CoilCl::Semer::PreliminaryAssert()
{
	/*if (itr != m_ast.cend()) {
		auto declExpr = std::dynamic_pointer_cast<DeclRefExpr>(itr.shared_ptr());
		if (!declExpr->IsResolved()) {
			auto resvName = declExpr->Identifier();
			if (std::any_of(staticLookup.cbegin(), staticLookup.cend(), [&resvName](const std::string& c) { return c == resvName; })) {
				printf("lolz");
			}
		}
	}*/

	//TODO: identifier resolving & scoping
	NamedDeclaration();
	ResolveIdentifier();
	BindPrototype();
	//TODO: type checking
	CheckDataType();

	return (*this);
}

CoilCl::Semer& CoilCl::Semer::StandardCompliance()
{
	return (*this);
}

template<typename _Ty>
std::shared_ptr<ASTNode> Closest(std::shared_ptr<ASTNode>& node)
{
	AST::ASTEqual<_Ty> eqOp;
	if (auto parent = node->Parent().lock()) {
		if (!eqOp(*parent.get())) {
			return Closest<_Ty>(parent);
		}

		return parent;
	}

	return nullptr;
}

#include <iostream>
// Extract identifiers from declarations and stash them per scoped block.
// All declaration nodes have an identifier, which could be empty.
void CoilCl::Semer::NamedDeclaration()
{
	AST::ASTEqual<TranslationUnitDecl> traunOp;
	AST::ASTDerived<Decl> drivdOp;
	OnMatch(m_ast.begin(), m_ast.end(), drivdOp, [&traunOp, this](AST::AST::iterator itr)
	{
		auto node = itr.shared_ptr();
		auto decl = std::dynamic_pointer_cast<Decl>(node);
		if (traunOp(*decl.get())) {
			return;
		}

		if (!decl->Identifier().empty()) {
			auto func = Closest<FunctionDecl>(node);
			if (func == nullptr) {
				auto block = Closest<CompoundStmt>(node);
				if (block == nullptr) {
					this->m_resolveList[decl->Identifier()] = node;
					std::cout << "Global declaration: " << decl->Identifier() << std::endl;
				}
				else {
					std::cout << "block declaration: " << decl->Identifier() << std::endl;
				}
			}
			else {
				std::cout << "Local declaration: " << decl->Identifier() << std::endl;
			}
		}
	});
}

void CoilCl::Semer::ResolveIdentifier()
{
	/*AST::ASTEqual<DeclRefExpr> eqOp;
	OnMatch(m_ast.begin(), m_ast.end(), eqOp, [this](AST::AST::iterator itr)
	{
		auto decl = std::dynamic_pointer_cast<DeclRefExpr>(itr.shared_ptr());
		if (!decl->IsResolved()) {

		}
	});*/


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

void CoilCl::Semer::BindPrototype()
{
	Stash<ASTNode> m_resolFuncProto;

	AST::ASTEqual<FunctionDecl> eqOp;
	OnMatch(m_ast.begin(), m_ast.end(), eqOp, [&m_resolFuncProto](AST::AST::iterator itr)
	{
		auto func = std::dynamic_pointer_cast<FunctionDecl>(itr.shared_ptr());
		if (func->IsPrototypeDefinition()) {
			m_resolFuncProto.Enlist(func);
		}
		else {
			auto funcProto = m_resolFuncProto.Resolve<FunctionDecl>([&func](std::shared_ptr<FunctionDecl>& funcPtr) -> bool
			{
				return funcPtr->Identifier() == func->Identifier()
					&& funcPtr->IsPrototypeDefinition();
			});

			if (!funcProto) {
				return;
			}

			func->BindPrototype(funcProto);
			funcProto->RegisterCaller();
		}
	});
}

void CoilCl::Semer::CheckDataType()
{
	//TODO: implicit cast
}
