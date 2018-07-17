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

// Global definitions occupy index 0 in the definitions list.
#define GLOBAL_DEFS	0

using namespace Cry::Algorithm;

class SemanticException;

// Inject implicit cast with converter.
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

CoilCl::Semer::Semer(std::shared_ptr<CoilCl::Profile>& profile, AST::AST&& ast, ConditionTracker::Tracker& tracker)
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

		return Util::NodeCast<NodeType>(parent);
	}

	return nullptr;
}

// Test if the return statement type matches the function return type, if not inject a converter.
template<size_t Idx = 0, typename ParentNode, typename ChildNode>
void IsConversionRequired(std::shared_ptr<ParentNode> parent, std::shared_ptr<ChildNode> child)
{
	// Skip if an converter is already injected.
	if (AST::NodeID::IMPLICIT_CONVERTION_EXPR_ID == child->Label()) { return; }

	auto retBase = std::dynamic_pointer_cast<Returnable>(parent);
	auto retInit = std::dynamic_pointer_cast<Returnable>(child);
	if (retBase && retInit) {
		const Typedef::TypeFacade& baseType = retBase->ReturnType();
		const Typedef::TypeFacade& initType = retInit->ReturnType();

		assert(baseType.HasValue() && initType.HasValue());
		if (baseType != initType) {
			InjectConverter<Idx>(parent, child, baseType, initType);
		}
	}
}

// ...
template<typename ParentNode, typename ChildNode>
void SetConversion(std::shared_ptr<ParentNode> parent, std::shared_ptr<ChildNode> child)
{
	assert(AST::NodeID::CAST_EXPR_ID == parent->Label());

	auto retBase = std::dynamic_pointer_cast<Returnable>(parent);
	auto retInit = std::dynamic_pointer_cast<Returnable>(child);
	if (retBase && retInit) {
		const Typedef::TypeFacade& baseType = retBase->ReturnType();
		const Typedef::TypeFacade& initType = retInit->ReturnType();

		assert(baseType.HasValue() && initType.HasValue());
		Conv::Cast::Tag m_tag = Conv::Cast::Tag::NONE_CAST;
		if (baseType != initType) {
			m_tag = Conv::Cast::Transmute(baseType, initType);
		}

		parent->SetConverterOperation(m_tag);
	}
}

} // namespace

// Resolve all static expresions such as native type size calculations
// and inject the expression result back into the copied tree.
CoilCl::Semer& CoilCl::Semer::StaticResolve()
{
	const std::array<std::string, 1> staticLookup{ "sizeof" };

	AST::Compare::Equal<BuiltinExpr> eqOp;
	MatchIf(m_ast.begin(), m_ast.end(), eqOp, [&staticLookup](AST::AST::iterator itr)
	{
		auto builtinExpr = Util::NodeCast<BuiltinExpr>(itr.shared_ptr());
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
		auto func = Util::NodeCast<FunctionDecl>(itr.shared_ptr());
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
		auto decl = Util::NodeCast<Decl>(node);

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
		auto decl = Util::NodeCast<DeclRefExpr>(node);
		if (!decl->IsResolved()) {
			auto func = Closest<FunctionDecl>(node);
			if (!func) {
				auto block = Closest<CompoundStmt>(node);
				if (!block) {
					auto binder = this->m_resolveList[GLOBAL_DEFS].find(decl->Identifier());
					if (binder == this->m_resolveList[GLOBAL_DEFS].end()) {
						semfmt % decl->Identifier();
						throw SemanticException{ semfmt.str().c_str(), 0, 0 };
					}

					decl->Resolve(binder->second);
					Util::NodeCast<Decl>(binder->second)->RegisterCaller();
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
				Util::NodeCast<Decl>(binder->second)->RegisterCaller();
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
		auto func = Util::NodeCast<FunctionDecl>(itr.shared_ptr());
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
// operators and functions based on connected nodes. The order of processing
// is important as returnable objects are processed from the bottom up.
void CoilCl::Semer::DeduceTypes()
{
	// Set signature in function definition.
	AST::Compare::Equal<FunctionDecl> eqOp;
	AST::Compare::Equal<VariadicDecl> eqVaria;
	MatchIf(m_ast.begin(), m_ast.end(), eqOp, [&eqVaria](AST::AST::iterator itr)
	{
		std::vector<Typedef::TypeFacade> paramTypeList;

		// Skip if there are no parameters, or signature was already set.
		auto func = Util::NodeCast<FunctionDecl>(itr.shared_ptr());
		if (!func->ParameterStatement() || func->HasSignature()) {
			return;
		}

		auto parameters = func->ParameterStatement()->Children();
		for (auto it = parameters.begin(); it != parameters.end(); ++it) {
			if (auto child = it->lock()) {
				if (eqVaria(PTR_NATIVE(child)) && it != parameters.end() - 1) {
					throw SemanticException{ "no argument expected after '...'", 0, 0 };
				}
				paramTypeList.push_back(Util::NodeCast<Returnable>(child)->ReturnType());
			}
		}

		if (!paramTypeList.empty()) {
			func->SetSignature(std::move(paramTypeList));
		}
	});

	// Set return type on call expression.
	AST::Compare::Equal<CallExpr> eqCall;
	MatchIf(m_ast.begin(), m_ast.end(), eqCall, [](AST::AST::iterator itr)
	{
		auto call = Util::NodeCast<CallExpr>(itr.shared_ptr());
		assert(call->FuncDeclRef()->IsResolved());
		assert(call->FuncDeclRef()->HasReturnType());

		if (!call->HasReturnType()) {
			call->SetReturnType(call->FuncDeclRef()->Reference()->ReturnType());
		}

		assert(call->HasReturnType());
	});

	// Set return type on enum declaration variable.
	AST::Compare::Derived<EnumConstantDecl> enumOp;
	MatchIf(m_ast.begin(), m_ast.end(), enumOp, [](AST::AST::iterator itr)
	{
		auto enumDecl = Util::NodeCast<EnumConstantDecl>(itr.shared_ptr());
		if (!enumDecl->Children().empty() && !enumDecl->HasReturnType()) {
			auto decl = enumDecl->Children().front().lock();
			if (!decl) { return; }

			auto rdecl = Util::NodeCast<Returnable>(decl);
			if (!rdecl->HasReturnType()) {
				throw SemanticException{ "initializer must be integer constant expression", 0, 0 };
			}

			// Enum initializer must be type of integer.
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

		assert(enumDecl->HasReturnType());
	});

	// Set return type on operators and delegate type down the tree. The operator type
	// is deducted from the first expression with an return type.
	AST::Compare::Derived<Operator> drvOp;
	MatchIf(m_ast.begin(), m_ast.end(), drvOp, [](AST::AST::iterator itr)
	{
		auto opr = Util::NodeCast<Operator>(itr.shared_ptr());

		AST::AST delegate{ opr };
		AST::Compare::Derived<Returnable> drvRet;
		MatchIf(delegate.begin(), delegate.end(), drvRet, [&opr](AST::AST::iterator del_itr)
		{
			auto retType = Util::NodeCast<Returnable>(del_itr.shared_ptr());
			if (retType->HasReturnType() && !opr->HasReturnType()) {
				opr->SetReturnType(retType->ReturnType());
			}
		});

		assert(opr->HasReturnType());
	});

	// Set return type on call expression.
	AST::Compare::Equal<ParenExpr> eqExpr;
	MatchIf(m_ast.begin(), m_ast.end(), eqExpr, [](AST::AST::iterator itr)
	{
		auto paren = Util::NodeCast<ParenExpr>(itr.shared_ptr());

		AST::AST delegate{ paren };
		AST::Compare::Derived<Returnable> drvRet;
		MatchIf(delegate.begin(), delegate.end(), drvRet, [&paren](AST::AST::iterator del_itr)
		{
			auto retType = Util::NodeCast<Returnable>(del_itr.shared_ptr());
			if (retType->HasReturnType() && !paren->HasReturnType()) {
				paren->SetReturnType(retType->ReturnType());
			}
		});

		assert(paren->HasReturnType());
	});

	// ...
	AST::Compare::Equal<CastExpr> eqCast;
	MatchIf(m_ast.begin(), m_ast.end(), eqCast, [](AST::AST::iterator itr)
	{
		auto cast = Util::NodeCast<CastExpr>(itr.shared_ptr());
		SetConversion(cast, cast->Expression());
	});
}

// Check if all datatypes are convertible and inject type conversions in the tree
// when two types can be casted. This method should only perform readonly operations
// on the tree.
void CoilCl::Semer::CheckDataType()
{
	// Compare function with its prototype, if exist.
	AST::Compare::Equal<FunctionDecl> eqFuncOp;
	MatchIf(m_ast.begin(), m_ast.end(), eqFuncOp, [](AST::AST::iterator itr)
	{
		auto func = Util::NodeCast<FunctionDecl>(itr.shared_ptr());
		if (func->IsPrototypeDefinition() || !func->HasPrototypeDefinition()) {
			return;
		}

		// Return type must match on function definition and prototype.
		if (func->PrototypeDefinition()->ReturnType() != func->ReturnType()) {
			throw SemanticException{ "conflicting types for 'x'", 0, 0 };
		}

		// Function signature must match on definition and prototype.
		if (func->PrototypeDefinition()->Signature() != func->Signature()) {
			throw SemanticException{ "conflicting types for 'x'", 0, 0 };
		}
	});

	// Match function signature with caller.
	AST::Compare::Equal<CallExpr> eqCallOp;
	MatchIf(m_ast.begin(), m_ast.end(), eqCallOp, [](AST::AST::iterator itr)
	{
		auto call = Util::NodeCast<CallExpr>(itr.shared_ptr());
		auto func = std::dynamic_pointer_cast<FunctionDecl>(call->FuncDeclRef()->Reference());
		assert(call->FuncDeclRef()->IsResolved());

		// Early exit.
		if (!call->HasArguments() && !func->HasSignature()) {
			return;
		}

		auto arguments = call->HasArguments()
			? call->ArgumentStatement()->Children()
			: std::vector<std::weak_ptr<AST::ASTNode>>{};

		// Make an exception for variadic argument.
		bool canHaveTooMany = true;
		if (func->HasSignature() && func->Signature().back().Type() == typeid(Typedef::VariadicType)) { //TODO: FIX
			canHaveTooMany = false;
		}

		// If argument size is off, throw exception.
		if (func->Signature().size() > arguments.size()) {
			throw SemanticException{ "too few arguments to function call, expected at least 0, have 0", 0, 0 };
		}
		else if (func->Signature().size() < arguments.size() && canHaveTooMany) {
			throw SemanticException{ "too many arguments to function call, expected 0, have 0", 0, 0 };
		}
	});

	//TODO: move to DeduceTypes?
	// Inject type converter in vardecl.
	AST::Compare::Equal<VarDecl> eqVar;
	MatchIf(m_ast.begin(), m_ast.end(), eqVar, [](AST::AST::iterator itr)
	{
		auto decl = Util::NodeCast<VarDecl>(itr.shared_ptr());

		for (const auto& wInitializer : decl->Children()) {
			if (auto initializer = wInitializer.lock()) {
				IsConversionRequired(decl, initializer);
			}
		}
	});

	//TODO: move to DeduceTypes?
	// Inject type converter in operator.
	AST::Compare::Derived<BinaryOperator> eqOp;
	MatchIf(m_ast.begin(), m_ast.end(), eqOp, [](AST::AST::iterator itr)
	{
		enum
		{
			OperatorLHS = 0,
			OperatorRHS = 1,
		};

		auto opr = Util::NodeCast<BinaryOperator>(itr.shared_ptr());

		auto intializerLHS = opr->Children().front().lock();
		if (intializerLHS) {
			IsConversionRequired<OperatorLHS>(opr, intializerLHS);
		}

		auto intializerRHS = opr->Children().back().lock();
		if (intializerRHS) {
			IsConversionRequired<OperatorRHS>(opr, intializerRHS);
		}
	});

	// Set return type on cast expression.
	/*AST::Compare::Equal<CastExpr> eqCast;
	MatchIf(m_ast.begin(), m_ast.end(), eqCast, [](AST::AST::iterator itr)
	{
	auto cast = Util::NodeCast<CastExpr>(itr.shared_ptr());
	auto retType = Util::NodeCast<Returnable>(cast->Expression());
	assert(retType->HasReturnType());
	cast->SetReturnType(retType->ReturnType());
	});*/

	//TODO: move to DeduceTypes?
	// Check function return type.
	AST::Compare::Equal<ReturnStmt> eqRet;
	MatchIf(m_ast.begin(), m_ast.end(), eqRet, [&eqFuncOp](AST::AST::iterator itr)
	{
		auto node = itr.shared_ptr();
		auto stmt = Util::NodeCast<ReturnStmt>(node);
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

		IsConversionRequired(stmt, intializer);
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
