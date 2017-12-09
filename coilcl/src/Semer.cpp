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

class SemanticException : public std::exception
{
public:
	SemanticException() noexcept = default;

	explicit SemanticException(char const* const message, int line, int column) noexcept
		: m_line{ line }
		, m_column{ column }
	{
		std::stringstream ss;
		ss << "Semantic error: " << message;
		ss << " " << line << ':' << column;
		_msg = ss.str();
	}

	virtual int Line() const noexcept
	{
		return m_line;
	}

	virtual int Column() const noexcept
	{
		return m_column;
	}

	virtual const char *what() const noexcept
	{
		return _msg.c_str();
	}

protected:
	std::string _msg;

private:
	int m_line;
	int m_column;
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
	NamedDeclaration();
	ResolveIdentifier();
	BindPrototype();
	DeduceTypes();
	CheckDataType();

	//ASSERTION_PASSED

	return (*this);
}

void CoilCl::Semer::FuncToSymbol(std::function<void(const std::string, const std::shared_ptr<ASTNode>& node)> insert)
{
	AST::ASTEqual<FunctionDecl> eqOp;
	OnMatch(m_ast.begin(), m_ast.end(), eqOp, [&insert](AST::AST::iterator itr)
	{
		auto func = std::dynamic_pointer_cast<FunctionDecl>(itr.shared_ptr());
		if (func->ReturnType()->IsInline() || func->IsPrototypeDefinition()) {
			return;
		}

		insert(func->Identifier(), func);
	});
}

//#include <iostream>
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
					this->m_resolveList[0][decl->Identifier()] = node;
					//std::cout << "Global declaration [0]: " << decl->Identifier() << std::endl;
				}
				else {
					throw std::exception{};//TODO
				}
			}
			else {
				this->m_resolveList[func->Id()][decl->Identifier()] = node;
				//std::cout << "Local declaration [" + std::to_string(func->Id()) + "]: " << decl->Identifier() << std::endl;
			}
		}
	});
}

void CoilCl::Semer::ResolveIdentifier()
{
	AST::ASTEqual<DeclRefExpr> eqOp;
	OnMatch(m_ast.begin(), m_ast.end(), eqOp, [=](AST::AST::iterator itr)
	{
		auto node = itr.shared_ptr();
		auto decl = std::dynamic_pointer_cast<DeclRefExpr>(node);
		if (!decl->IsResolved()) {
			auto func = Closest<FunctionDecl>(node);
			if (func == nullptr) {
				auto block = Closest<CompoundStmt>(node);
				if (block == nullptr) {
					auto binder = this->m_resolveList[0].find(decl->Identifier());
					if (binder == this->m_resolveList[0].end()) {
						throw SemanticException{ "use of undeclared identifier'x'", 0, 0 };
					}

					decl->Resolve(binder->second);
					std::dynamic_pointer_cast<Decl>(binder->second)->RegisterCaller();
				}
				else {
					throw std::exception{};//TODO
				}
			}
			else {
				auto binder = this->m_resolveList[func->Id()].find(decl->Identifier());
				if (binder == this->m_resolveList[func->Id()].end()) {
					binder = this->m_resolveList[0].find(decl->Identifier());
					if (binder == this->m_resolveList[0].end()) {
						throw SemanticException{ "use of undeclared identifier'x'", 0, 0 };
					}
				}

				decl->Resolve(binder->second);
				std::dynamic_pointer_cast<Decl>(binder->second)->RegisterCaller();
			}
		}
	});
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

			if (funcProto) {
				func->BindPrototype(funcProto);
				funcProto->RegisterCaller();
			}
		}
	});
}

void CoilCl::Semer::DeduceTypes()
{
	AST::ASTEqual<FunctionDecl> eqOp;
	AST::ASTEqual<VariadicDecl> eqVaria;
	OnMatch(m_ast.begin(), m_ast.end(), eqOp, [&eqVaria](AST::AST::iterator itr)
	{
		std::vector<AST::TypeFacade> paramTypeList;

		auto func = std::dynamic_pointer_cast<FunctionDecl>(itr.shared_ptr());
		auto parameters = func->ParameterStatement()->Children();
		for (auto it = parameters.begin(); it != parameters.end(); ++it) {
			if (auto child = it->lock()) {
				if (eqVaria(*child.get()) && it != parameters.end() - 1) {
					throw SemanticException{ "no argument expected after '...'", 0, 0 };
				}
				paramTypeList.push_back(std::dynamic_pointer_cast<Decl>(child)->ReturnType());
			}
		}

		if (!paramTypeList.empty()) {
			func->SetSignature(std::move(paramTypeList));
		}
	});
}

void CoilCl::Semer::CheckDataType()
{
	AST::ASTEqual<FunctionDecl> eqFuncOp;
	AST::ASTEqual<CallExpr> eqCallOp;

	// Compare function with its prototype, if exist
	OnMatch(m_ast.begin(), m_ast.end(), eqFuncOp, [](AST::AST::iterator itr)
	{
		auto func = std::dynamic_pointer_cast<FunctionDecl>(itr.shared_ptr());
		if (func->IsPrototypeDefinition() || !func->HasPrototypeDefinition()) {
			return;
		}

		// Return type must match
		if (func->PrototypeDefinition()->ReturnType() != func->ReturnType()) {
			throw SemanticException{ "conflicting types for 'x'", 0, 0 };
		}

		// Function signature must match
		if (func->PrototypeDefinition()->Signature() != func->Signature()) {
			throw SemanticException{ "conflicting types for 'x'", 0, 0 };
		}
	});

	// Match function signature with caller
	OnMatch(m_ast.begin(), m_ast.end(), eqCallOp, [](AST::AST::iterator itr)
	{
		auto call = std::dynamic_pointer_cast<CallExpr>(itr.shared_ptr());
		auto arguments = call->ArgumentStatement()->Children();
		assert(call->FuncDeclRef()->IsResolved());

		auto func = std::dynamic_pointer_cast<FunctionDecl>(call->FuncDeclRef()->Reference());

		// Make an exception for variadic argument
		bool canHaveTooMany = true;
		if (func->Signature().back().Type() == typeid(Typedef::VariadicType)) {
			canHaveTooMany = false;
		}

		if (func->Signature().size() > arguments.size()) {
			throw SemanticException{ "too few arguments to function call, expected at least 0, have 0", 0, 0 };
		}
		else if (func->Signature().size() < arguments.size() && canHaveTooMany) {
			throw SemanticException{ "too many arguments to function call, expected 0, have 0", 0, 0 };
		}
	});

	//TODO: Check expressions & return types
}

CoilCl::Semer& CoilCl::Semer::StandardCompliance()
{
	return (*this);
}

CoilCl::Semer& CoilCl::Semer::PedanticCompliance()
{
	return (*this);
}
