// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <Cry/Algorithm.h>

#include <boost/format.hpp>

#include "Semer.h"
#include "ValueHelper.h"
#include "Converter.h"

#define PTR_NATIVE(p) (*(p).get())

// Global definitions occupy index 0 in the definitions list
#define GLOBAL_DEFS	0

using namespace Cry::Algorithm;

class SemanticException;

template<size_t Idx = 0, typename ConvertType, typename ParentType, typename ChildType>
void InjectConverter(std::shared_ptr<ParentType> parent, std::shared_ptr<ChildType> child, ConvertType baseType, ConvertType initType)
{
	try {
		Conv::Cast::Tag methodTag = Conv::Cast::Transmute(baseType, initType);
		auto converter = AST::MakeASTNode<ImplicitConvertionExpr>(child, methodTag);
		converter->SetReturnType(baseType);
		parent->Emplace(Idx, converter);
	}
	catch (Conv::ConverterException& e) {
		throw SemanticException{ e.what(), 0, 0 };
	}
}

//TODO: replace by Cry::except...
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

CoilCl::Semer::Semer(std::shared_ptr<CoilCl::Profile>& profile, AST::AST&& ast, ConditionTracker::Tracker tracker)
	: Stage{ this, StageType::Type::SemanticAnalysis, tracker }
	, m_profile{ profile }
	, m_ast{ std::move(ast) }
{
}

CoilCl::Semer& CoilCl::Semer::CheckCompatibility()
{
	//TODO
	return (*this);
}

namespace
{

template<typename NodeType>
std::shared_ptr<NodeType> Closest(std::shared_ptr<CoilCl::AST::ASTNode>& node)
{
	AST::Compare::Equal<NodeType> eqOp;
	if (auto parent = node->Parent().lock()) {
		if (!eqOp(PTR_NATIVE(parent))) {
			return Closest<NodeType>(parent);
		}

		return std::dynamic_pointer_cast<NodeType>(parent);
	}

	return nullptr;
}

template<size_t Idx = 0, typename _ConvTy, typename _ParentTy, typename _ChildTy>
void IsConversionRequired(std::shared_ptr<_ParentTy> parent, std::shared_ptr<_ChildTy> child, _ConvTy baseType)
{
	// Skip if an converter is already injected.
	if (std::dynamic_pointer_cast<ImplicitConvertionExpr>(child) != nullptr) {
		return;
	}

	if (auto ret = std::dynamic_pointer_cast<Returnable>(child)) {
		const Typedef::TypeFacade& initType = ret->ReturnType();

		assert(initType.HasValue());
		if (baseType != initType) {
			InjectConverter<Idx>(parent, child, baseType, initType);
		}
	}
}

} // namespace

// Resolve all static expresions such as native type size calculations
// and inject the expression result back into the copied tree
CoilCl::Semer& CoilCl::Semer::StaticResolve()
{
	const std::array<std::string, 1> staticLookup{ "sizeof" };

	AST::Compare::Equal<BuiltinExpr> eqOp;
	MatchIf(m_ast.begin(), m_ast.end(), eqOp, [&staticLookup](AST::AST::iterator itr)
	{
		auto builtinExpr = std::dynamic_pointer_cast<BuiltinExpr>(itr.shared_ptr());
		auto declRefName = builtinExpr->FuncDeclRef()->Identifier();

		if (std::any_of(staticLookup.cbegin(), staticLookup.cend(), [&declRefName](const std::string& c) { return c == declRefName; })) {

			// If expression, evaluate outcome
			if (builtinExpr->Expression()) {
				CryImplExcept(); //TODO
			}
			// No expression, use typename
			else {
				// Replace static builtin operation with integer result
				auto m_data = Util::MakeInt(static_cast<int>(builtinExpr->TypeName().Size()));
				auto literal = CoilCl::AST::MakeASTNode<IntegerLiteral>(std::move(m_data));

				// Emplace current object on existing
				if (auto parent = builtinExpr->Parent().lock()) {
					auto parentChildren = parent->Children();

					auto selfListItem = std::find_if(parentChildren.begin(), parentChildren.end(), [=](std::weak_ptr<CoilCl::AST::ASTNode>& wPtr)
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

	this->CompletePhase(ConditionTracker::STATIC_RESOLVED);
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

	this->CompletePhase(ConditionTracker::ASSERTION_PASSED);
	return (*this);
}

void CoilCl::Semer::FuncToSymbol(std::function<void(const std::string, const std::shared_ptr<CoilCl::AST::ASTNode>& node)> insert)
{
	AST::Compare::Equal<FunctionDecl> eqOp;
	MatchIf(m_ast.begin(), m_ast.end(), eqOp, [&insert](AST::AST::iterator itr)
	{
		auto func = std::dynamic_pointer_cast<FunctionDecl>(itr.shared_ptr());
		if (func->ReturnType()->IsInline() || func->IsPrototypeDefinition()) {
			return;
		}

		insert(func->Identifier(), func);
	});
}

// Extract identifiers from declarations and stash them per scoped block.
// All declaration nodes have an identifier, which could be empty.
void CoilCl::Semer::NamedDeclaration()
{
	AST::Compare::Equal<TranslationUnitDecl> traunOp;
	AST::Compare::Derived<Decl> drivdOp;
	MatchIf(m_ast.begin(), m_ast.end(), drivdOp, [&traunOp, this](AST::AST::iterator itr)
	{
		auto node = itr.shared_ptr();
		auto decl = std::dynamic_pointer_cast<Decl>(node);

		// Ignore translation unit declaration
		if (traunOp(PTR_NATIVE(decl))) {
			return;
		}

		if (!decl->Identifier().empty()) {
			auto func = Closest<FunctionDecl>(node);
			if (func == nullptr) {
				auto block = Closest<CompoundStmt>(node);
				if (block == nullptr) {
					this->m_resolveList[GLOBAL_DEFS][decl->Identifier()] = node;
					//std::cout << "Global declaration [0]: " << decl->Identifier() << std::endl;
				}
				else {
					throw SemanticException{ "illegal compound outside function scope", 0, 0 };
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
	MatchIf(m_ast.begin(), m_ast.end(), eqOp, [=](AST::AST::iterator itr)
	{
		boost::format semfmt{ "use of undeclared identifier '%1%'" };

		auto node = itr.shared_ptr();
		auto decl = std::dynamic_pointer_cast<DeclRefExpr>(node);
		if (!decl->IsResolved()) {
			auto func = Closest<FunctionDecl>(node);
			if (func == nullptr) {
				auto block = Closest<CompoundStmt>(node);
				if (block == nullptr) {
					auto binder = this->m_resolveList[GLOBAL_DEFS].find(decl->Identifier());
					if (binder == this->m_resolveList[GLOBAL_DEFS].end()) {
						semfmt % decl->Identifier();
						throw SemanticException{ semfmt.str().c_str(), 0, 0 };
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
					binder = this->m_resolveList[GLOBAL_DEFS].find(decl->Identifier());
					if (binder == this->m_resolveList[GLOBAL_DEFS].end()) {
						semfmt % decl->Identifier();
						throw SemanticException{ semfmt.str().c_str(), 0, 0 };
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
	Stash<CoilCl::AST::ASTNode> m_resolFuncProto;

	MatchIf(m_ast.begin(), m_ast.end(), eqOp, [&m_resolFuncProto](AST::AST::iterator itr)
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

// Determine and set the return types and signatures for expressions,
// operators and functions based on connected nodes
void CoilCl::Semer::DeduceTypes()
{
	// Set signature in function definition
	AST::Compare::Equal<FunctionDecl> eqOp;
	AST::Compare::Equal<VariadicDecl> eqVaria;
	MatchIf(m_ast.begin(), m_ast.end(), eqOp, [&eqVaria](AST::AST::iterator itr)
	{
		std::vector<Typedef::TypeFacade> paramTypeList;

		auto func = std::dynamic_pointer_cast<FunctionDecl>(itr.shared_ptr());
		if (func->ParameterStatement() == nullptr || func->HasSignature()) {
			return;
		}

		auto parameters = func->ParameterStatement()->Children();
		for (auto it = parameters.begin(); it != parameters.end(); ++it) {
			if (auto child = it->lock()) {
				if (eqVaria(PTR_NATIVE(child)) && it != parameters.end() - 1) {
					throw SemanticException{ "no argument expected after '...'", 0, 0 };
				}
				paramTypeList.push_back(std::dynamic_pointer_cast<Returnable>(child)->ReturnType());
			}
		}

		if (!paramTypeList.empty()) {
			func->SetSignature(std::move(paramTypeList));
		}
	});

	// Set return type on call expression
	AST::Compare::Equal<CallExpr> eqCall;
	MatchIf(m_ast.begin(), m_ast.end(), eqCall, [](AST::AST::iterator itr)
	{
		auto call = std::dynamic_pointer_cast<CallExpr>(itr.shared_ptr());
		assert(call->FuncDeclRef()->IsResolved());
		assert(call->FuncDeclRef()->HasReturnType());

		if (!call->HasReturnType()) {
			call->SetReturnType(call->FuncDeclRef()->Reference()->ReturnType());
		}
	});

	// Set return type on operators and delegate type down the tree. The operator type
	// is deducted from the first expression with an return type
	AST::Compare::Derived<Operator> drvOp;
	MatchIf(m_ast.begin(), m_ast.end(), drvOp, [](AST::AST::iterator itr)
	{
		auto opr = std::dynamic_pointer_cast<Operator>(itr.shared_ptr());

		AST::AST delegate{ opr };
		AST::Compare::Derived<Returnable> baseNodeOp;
		MatchIf(delegate.begin(), delegate.end(), baseNodeOp, [&opr](AST::AST::iterator del_itr)
		{
			auto retType = std::dynamic_pointer_cast<Returnable>(del_itr.shared_ptr());
			if (retType->HasReturnType() && !opr->HasReturnType()) {
				opr->SetReturnType(retType->ReturnType());
			}
		});

		assert(opr->HasReturnType());

		AST::Compare::Derived<Returnable> drvOp2;
		MatchIf(delegate.begin(), delegate.end(), drvOp2, [&opr](AST::AST::iterator del_itr)
		{
			auto retType = std::dynamic_pointer_cast<Returnable>(del_itr.shared_ptr());
			if (!retType->HasReturnType()) {
				retType->SetReturnType(opr->ReturnType());
			}
		});
	});

	AST::Compare::Derived<EnumConstantDecl> enumOp;
	MatchIf(m_ast.begin(), m_ast.end(), enumOp, [](AST::AST::iterator itr)
	{
		auto enumDecl = std::dynamic_pointer_cast<EnumConstantDecl>(itr.shared_ptr());
		if (!enumDecl->Children().empty() && !enumDecl->HasReturnType()) {
			auto decl = enumDecl->Children().front().lock();
			if (!decl) {
				return;
			}

			auto rdecl = std::dynamic_pointer_cast<Returnable>(decl);
			if (!rdecl->HasReturnType()) {
				throw SemanticException{ "initializer must be integer constant expression", 0, 0 };
			}

			// Enum initializer must be type of integer
			if (!rdecl->ReturnType()->Equals(Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::INT).get())
				&& !rdecl->ReturnType()->Equals(Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::CHAR).get())) {
				throw SemanticException{ "initializer must be integer constant expression", 0, 0 };
			}

			enumDecl->SetReturnType(rdecl->ReturnType());
		}
		else {
			auto integer = std::dynamic_pointer_cast<Typedef::TypedefBase>(Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::INT));
			enumDecl->SetReturnType(Typedef::TypeFacade{ integer });
		}
	});
}

// Check if all datatypes are convertible and inject type conversions in the tree
// when two types can be casted
void CoilCl::Semer::CheckDataType()
{
	AST::Compare::Equal<FunctionDecl> eqFuncOp;
	AST::Compare::Equal<CallExpr> eqCallOp;

	// Compare function with its prototype, if exist
	MatchIf(m_ast.begin(), m_ast.end(), eqFuncOp, [](AST::AST::iterator itr)
	{
		auto func = std::dynamic_pointer_cast<FunctionDecl>(itr.shared_ptr());
		if (func->IsPrototypeDefinition() || !func->HasPrototypeDefinition()) {
			return;
		}

		// Return type must match on function definition and prototype
		if (func->PrototypeDefinition()->ReturnType() != func->ReturnType()) {
			throw SemanticException{ "conflicting types for 'x'", 0, 0 };
		}

		// Function signature must match on definition and prototype
		if (func->PrototypeDefinition()->Signature() != func->Signature()) {
			throw SemanticException{ "conflicting types for 'x'", 0, 0 };
		}
	});

	// Match function signature with caller
	MatchIf(m_ast.begin(), m_ast.end(), eqCallOp, [](AST::AST::iterator itr)
	{
		auto call = std::dynamic_pointer_cast<CallExpr>(itr.shared_ptr());
		auto func = std::dynamic_pointer_cast<FunctionDecl>(call->FuncDeclRef()->Reference());
		assert(call->FuncDeclRef()->IsResolved());

		// Early exit
		if (!call->HasArguments() && !func->HasSignature()) {
			return;
		}

		auto arguments = call->HasArguments()
			? call->ArgumentStatement()->Children()
			: std::vector<std::weak_ptr<AST::ASTNode>>{};

		// Make an exception for variadic argument
		bool canHaveTooMany = true;
		if (func->HasSignature() && func->Signature().back().Type() == typeid(Typedef::VariadicType)) { //TODO: FIX
			canHaveTooMany = false;
		}

		// If argument size is off, throw exception
		if (func->Signature().size() > arguments.size()) {
			throw SemanticException{ "too few arguments to function call, expected at least 0, have 0", 0, 0 };
		}
		else if (func->Signature().size() < arguments.size() && canHaveTooMany) {
			throw SemanticException{ "too many arguments to function call, expected 0, have 0", 0, 0 };
		}
	});

	// Inject type converter in vardecl
	AST::Compare::Equal<VarDecl> eqVar;
	MatchIf(m_ast.begin(), m_ast.end(), eqVar, [](AST::AST::iterator itr)
	{
		auto decl = std::dynamic_pointer_cast<VarDecl>(itr.shared_ptr());
		Typedef::TypeFacade baseType = decl->ReturnType();

		for (const auto& wIntializer : decl->Children()) {
			if (auto intializer = wIntializer.lock()) {
				IsConversionRequired(decl, intializer, baseType);
			}
		}
	});

	// Inject type converter in operator
	AST::Compare::Derived<BinaryOperator> eqOp;
	MatchIf(m_ast.begin(), m_ast.end(), eqOp, [](AST::AST::iterator itr)
	{
		enum
		{
			OperatorLHS = 0,
			OperatorRHS = 1,
		};

		auto opr = std::dynamic_pointer_cast<BinaryOperator>(itr.shared_ptr());
		Typedef::TypeFacade baseType = opr->ReturnType();

		auto intializerLHS = opr->Children().front().lock();
		if (intializerLHS) {
			IsConversionRequired<OperatorLHS>(opr, intializerLHS, baseType);
		}

		auto intializerRHS = opr->Children().back().lock();
		if (intializerRHS) {
			IsConversionRequired<OperatorRHS>(opr, intializerRHS, baseType);
		}
	});

	// Check function return type.
	AST::Compare::Equal<ReturnStmt> eqRet;
	MatchIf(m_ast.begin(), m_ast.end(), eqRet, [&eqFuncOp](AST::AST::iterator itr)
	{
		auto node = itr.shared_ptr();
		auto stmt = std::dynamic_pointer_cast<ReturnStmt>(node);
		auto func = Closest<FunctionDecl>(node);

		// No function found at return parent.
		if (!func) {
			throw SemanticException{ "return must be scoped by function declaration", 0, 0 };
		}

		// Function expected type from return while void was returned.
		if (!stmt->HasExpression() && !func->ReturnType()->Equals(Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::VOID_T).get())) {
			throw SemanticException{ "function declaration expected expression on return", 0, 0 };
		}

		// Function expects no returning type while value was returned.
		if (stmt->HasExpression() && func->ReturnType()->Equals(Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::VOID_T).get())) {
			throw SemanticException{ "unexpected expression on return", 0, 0 };
		}
		// If type is void and expresion is empty, continue.
		else if (func->ReturnType()->Equals(Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::VOID_T).get())) {
			return;
		}

		// Return type must be either operator, expression or literal.
		const auto intializer = stmt->Expression();
		AST::Compare::MultiDerive<Operator, Expr, Literal> baseNodeOp;
		if (!baseNodeOp(PTR_NATIVE(intializer))) {
			throw SemanticException{ "expected operator, expression or literal", 0, 0 };
		}

		auto baseType = func->ReturnType();
		IsConversionRequired(stmt, intializer, baseType);
	});
}

CoilCl::Semer& CoilCl::Semer::StandardCompliance()
{
	this->CompletePhase(ConditionTracker::COMPLIANT);
	return (*this);
}

CoilCl::Semer& CoilCl::Semer::PedanticCompliance()
{
	return (*this);
}
