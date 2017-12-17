#include "AST.h"
#include "ASTNode.h"
#include "Semer.h"
#include "Converter.h"

#define PTR_NATIVE(p) (*(p).get())

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

template<size_t _Idx = 0, typename _ConvTy, typename _ParentTy, typename _ChildTy>
void InjectConverter(std::shared_ptr<_ParentTy> parent, std::shared_ptr<_ChildTy> child, _ConvTy baseType, _ConvTy initType)
{
	try {
		Conv::Cast::Tag methodTag = Conv::Cast::Transmute(baseType, initType);
		auto converter = AST::MakeASTNode<ImplicitConvertionExpr>(child, methodTag);
		converter->SetReturnType(baseType);
		parent->Emplace(_Idx, converter);
	}
	catch (Conv::ConverterException& e) {
		throw SemanticException{ e.what(), 0, 0 };
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
std::shared_ptr<_Ty> Closest(std::shared_ptr<ASTNode>& node)
{
	AST::Compare::Equal<_Ty> eqOp;
	if (auto parent = node->Parent().lock()) {
		if (!eqOp(PTR_NATIVE(parent))) {
			return Closest<_Ty>(parent);
		}

		return std::dynamic_pointer_cast<_Ty>(parent);
	}

	return nullptr;
}

// Resolve all static expresions such as native type size calculations
// and inject the expression result back into the tree
CoilCl::Semer& CoilCl::Semer::StaticResolve()
{
	const std::array<std::string, 1> staticLookup{ "sizeof" };

	AST::Compare::Equal<BuiltinExpr> eqOp;
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
	AST::Compare::Equal<FunctionDecl> eqOp;
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
	AST::Compare::Equal<TranslationUnitDecl> traunOp;
	AST::Compare::Derived<Decl> drivdOp;

	OnMatch(m_ast.begin(), m_ast.end(), drivdOp, [&traunOp, this](AST::AST::iterator itr)
	{
		auto node = itr.shared_ptr();
		auto decl = std::dynamic_pointer_cast<Decl>(node);
		if (traunOp(PTR_NATIVE(decl))) {
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
	AST::Compare::Equal<DeclRefExpr> eqOp;

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
	AST::Compare::Equal<FunctionDecl> eqOp;
	Stash<ASTNode> m_resolFuncProto;

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
#include <iostream>
void CoilCl::Semer::DeduceTypes()
{
	// Set signature in function definition
	AST::Compare::Equal<FunctionDecl> eqOp;
	AST::Compare::Equal<VariadicDecl> eqVaria;
	OnMatch(m_ast.begin(), m_ast.end(), eqOp, [&eqVaria](AST::AST::iterator itr)
	{
		std::vector<AST::TypeFacade> paramTypeList;

		auto func = std::dynamic_pointer_cast<FunctionDecl>(itr.shared_ptr());
		if (func->ParameterStatement() == nullptr) {
			return;
		}

		auto parameters = func->ParameterStatement()->Children();
		for (auto it = parameters.begin(); it != parameters.end(); ++it) {
			if (auto child = it->lock()) {
				if (eqVaria(PTR_NATIVE(child)) && it != parameters.end() - 1) {
					throw SemanticException{ "no argument expected after '...'", 0, 0 };
				}
				paramTypeList.push_back(std::dynamic_pointer_cast<Decl>(child)->ReturnType());
			}
		}

		if (!paramTypeList.empty()) {
			func->SetSignature(std::move(paramTypeList));
		}
	});

	// Set return type on call expression
	AST::Compare::Equal<CallExpr> eqCall;
	OnMatch(m_ast.begin(), m_ast.end(), eqCall, [](AST::AST::iterator itr)
	{
		auto call = std::dynamic_pointer_cast<CallExpr>(itr.shared_ptr());
		auto func = std::dynamic_pointer_cast<FunctionDecl>(call->FuncDeclRef()->Reference());
		assert(call->FuncDeclRef()->IsResolved());

		call->SetReturnType(func->ReturnType());
	});

	// Set return type on operators
	AST::Compare::Derived<Operator> drvOp;
	OnMatch(m_ast.begin(), m_ast.end(), drvOp, [](AST::AST::iterator itr)
	{
		auto opr = std::dynamic_pointer_cast<Operator>(itr.shared_ptr());

		AST::AST delegate{ opr };
		AST::Compare::Derived<Expr> baseNodeOp;
		OnMatch(delegate.begin(), delegate.end(), baseNodeOp, [&opr](AST::AST::iterator del_itr)
		{
			auto type = std::dynamic_pointer_cast<Expr>(del_itr.shared_ptr())->ReturnType();
			opr->SetReturnType(type);
			return;
		});

		AST::Compare::MultiDerive<Operator, IntegerLiteral> drvOp2;
		OnMatch(delegate.begin(), delegate.end(), drvOp2, [&opr](AST::AST::iterator del_itr)
		{
			/*auto node = del_itr.shared_ptr();
			if (!node->HasReturnType()) {
				node->SetReturnType(opr->ReturnType());
			}*/
		});
	});
}

void CoilCl::Semer::CheckDataType()
{
	AST::Compare::Equal<FunctionDecl> eqFuncOp;
	AST::Compare::Equal<CallExpr> eqCallOp;

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
		auto func = std::dynamic_pointer_cast<FunctionDecl>(call->FuncDeclRef()->Reference());
		assert(call->FuncDeclRef()->IsResolved());

		auto arguments = call->ArgumentStatement()->Children();

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

	// Inject type converter in vardecl
	AST::Compare::Equal<VarDecl> eqVar;
	OnMatch(m_ast.begin(), m_ast.end(), eqVar, [](AST::AST::iterator itr)
	{
		auto decl = std::dynamic_pointer_cast<VarDecl>(itr.shared_ptr());
		AST::TypeFacade baseType = decl->ReturnType();

		for (const auto& wIntializer : decl->Children()) {
			if (auto intializer = wIntializer.lock()) {

				AST::TypeFacade initType;
				if (auto expr = std::dynamic_pointer_cast<Expr>(intializer)) {
					initType = expr->ReturnType();
				}
				//TODO: any literal
				else if (auto lit = std::dynamic_pointer_cast<IntegerLiteral>(intializer)) {
					initType = lit->ReturnType();
				}
				//TODO: any operator
				assert(initType.HasValue());

				// Find mutator if types do not match
				if (baseType != initType) {
					InjectConverter(decl, intializer, baseType, initType);
				}
			}
		}
	});

	// Inject type converter in operator
	AST::Compare::Derived<Operator> eqOp;
	OnMatch(m_ast.begin(), m_ast.end(), eqOp, [](AST::AST::iterator itr)
	{
		enum
		{
			OperatorLHS = 0,
			OperatorRHS = 1,
		};

		auto opr = std::dynamic_pointer_cast<Operator>(itr.shared_ptr());
		AST::TypeFacade baseType = opr->ReturnType();

		auto intializerLHS = opr->Children().front().lock();
		auto intializerRHS = opr->Children().back().lock();

		if (intializerLHS) {
			AST::TypeFacade initType;

			if (std::dynamic_pointer_cast<ImplicitConvertionExpr>(intializerLHS) != nullptr) {
				return;
			}

			if (auto expr = std::dynamic_pointer_cast<Expr>(intializerLHS)) {
				initType = expr->ReturnType();
			}
			//TODO: any literal
			else if (auto lit = std::dynamic_pointer_cast<IntegerLiteral>(intializerLHS)) {
				initType = lit->ReturnType();
			}
			//TODO: any operator
			assert(initType.HasValue());

			if (baseType != initType) {
				InjectConverter<OperatorLHS>(opr, intializerLHS, baseType, initType);
			}
		}

		if (intializerRHS) {
			AST::TypeFacade initType;

			if (std::dynamic_pointer_cast<ImplicitConvertionExpr>(intializerRHS) != nullptr) {
				return;
			}

			if (auto expr = std::dynamic_pointer_cast<Expr>(intializerRHS)) {
				initType = expr->ReturnType();
			}
			//TODO: any literal
			else if (auto lit = std::dynamic_pointer_cast<IntegerLiteral>(intializerRHS)) {
				initType = lit->ReturnType();
			}
			//TODO: any operator
			assert(initType.HasValue());

			// Find mutator if types do not match
			if (baseType != initType) {
				InjectConverter<OperatorRHS>(opr, intializerRHS, baseType, initType);
			}
		}
	});

	// Check function return type
	AST::Compare::Equal<ReturnStmt> eqRet;
	OnMatch(m_ast.begin(), m_ast.end(), eqRet, [&eqFuncOp](AST::AST::iterator itr)
	{
		auto node = itr.shared_ptr();
		auto stmt = std::dynamic_pointer_cast<ReturnStmt>(node);
		auto func = Closest<FunctionDecl>(node);

		// No function found at return parent
		if (func == nullptr) {
			throw SemanticException{ "return must be scoped by function declaration", 0, 0 };
		}

		// Function expected type from return
		if (!stmt->HasExpression() && !func->ReturnType()->Equals(Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::VOID).get())) {
			throw SemanticException{ "function declaration expected expression on return", 0, 0 };
		}

		// Function expects no returning type
		if (stmt->HasExpression() && func->ReturnType()->Equals(Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::VOID).get())) {
			throw SemanticException{ "unexpected expression on return", 0, 0 };
		}
		// If type is void and expresion is empty, continue
		else if (func->ReturnType()->Equals(Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::VOID).get())) {
			return;
		}

		auto intializer = stmt->Expression();

		AST::Compare::MultiDerive<Operator, Expr, IntegerLiteral> baseNodeOp;
		if (!baseNodeOp(PTR_NATIVE(intializer))) {
			throw SemanticException{ "expected operator, expression or literal", 0, 0 };
		}

		auto baseType = func->ReturnType();

		AST::TypeFacade initType;
		if (auto expr = std::dynamic_pointer_cast<Expr>(intializer)) {
			initType = expr->ReturnType();
		}
		else if (auto expr = std::dynamic_pointer_cast<Operator>(intializer)) {
			initType = expr->ReturnType();
		}
		//TODO: any literal
		else if (auto lit = std::dynamic_pointer_cast<IntegerLiteral>(intializer)) {
			initType = lit->ReturnType();
		}
		assert(initType.HasValue());

		// Find mutator if types do not match
		if (baseType != initType) {
			InjectConverter(stmt, intializer, baseType, initType);
		}
	});
}

CoilCl::Semer& CoilCl::Semer::StandardCompliance()
{
	return (*this);
}

CoilCl::Semer& CoilCl::Semer::PedanticCompliance()
{
	return (*this);
}
