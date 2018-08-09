// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

// Local includes.
#include "Semer.h"
#include "BuiltinRoutine.h"

// Project includes.
#include <CryCC/AST.h>

#include <Cry/Algorithm.h>

#include <boost/format.hpp>

#define PTR_NATIVE(p) (*(p).get())

// Global definitions occupy index 0 in the definitions list.
#define GLOBAL_DEFS	0

class SemanticException;

namespace
{

// Inject implicit cast with converter.
template<size_t Idx = 0, typename ConvertType, typename ParentType, typename ChildType>
void InjectConverter(std::shared_ptr<ParentType> parent, std::shared_ptr<ChildType> child, ConvertType baseType, ConvertType initType)
{
	try {
		CryCC::SubValue::Conv::Cast::Tag methodTag = CryCC::SubValue::Conv::Cast::Transmute(baseType, initType);
		auto converter = Util::MakeASTNode<CryCC::AST::ImplicitConvertionExpr>(child, methodTag);
		converter->SetReturnType(baseType);
		parent->Emplace(Idx, converter);
	}
	catch (CryCC::SubValue::Conv::ConverterException& e) {
		throw SemanticException{ e.what(), 0, 0 };
	}
}

} // namespace

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

namespace CoilCl
{

using namespace Cry::Algorithm;
using namespace CryCC::Program;
using namespace CryCC::AST;

Semer::Semer(std::shared_ptr<CoilCl::Profile>& profile, AST&& ast, ConditionTracker::Tracker& tracker)
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

// Test if the return statement type matches the function return type, if not inject a converter.
template<size_t Idx = 0, typename ParentNode, typename ChildNode>
void IsConversionRequired(std::shared_ptr<ParentNode> parent, std::shared_ptr<ChildNode> child)
{
	// Skip if an converter is already injected.
	if (NodeID::IMPLICIT_CONVERTION_EXPR_ID == child->Label()) { return; }

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
	assert(NodeID::CAST_EXPR_ID == parent->Label());

	auto retBase = std::dynamic_pointer_cast<Returnable>(parent);
	auto retInit = std::dynamic_pointer_cast<Returnable>(child);
	if (retBase && retInit) {
		const Typedef::TypeFacade& baseType = retBase->ReturnType();
		const Typedef::TypeFacade& initType = retInit->ReturnType();

		assert(baseType.HasValue() && initType.HasValue());
		CryCC::SubValue::Conv::Cast::Tag m_tag = CryCC::SubValue::Conv::Cast::Tag::NONE_CAST;
		if (baseType != initType) {
			m_tag = CryCC::SubValue::Conv::Cast::Transmute(baseType, initType);
		}

		parent->SetConverterOperation(m_tag);
	}
}

} // namespace

// Run all semantic checks that defines the language,
// this comprises type checking, object scope validation,
// implicit casting and identifier resolving.
Semer& Semer::PreliminaryAssert()
{
	//
	// Annotate tree.
	//

	NamedDeclaration();
	ResolveIdentifier();
	StaticResolve();
	BindPrototype();
	DeduceTypes();

	//
	// Verify tree.
	//

	CheckDataType();
	IllFormedConstruction();

	this->CompletePhase(ConditionTracker::ASSERTION_PASSED);
	return (*this);
}

#define BUILTIN_ROUTINE(r) \
	if (declRefName == #r) { \
		BuiltinRoutine::static_##r(builtinExpr); \
	}
#define RESERVE_BUILTIN_ROUTINE(r) \
	this->m_resolveList[GLOBAL_DEFS][r] = nullptr;

// Resolve all static expresions, and remove the result with the call.
void Semer::StaticResolve()
{
	Compare::Equal<BuiltinExpr> eqOp;
	MatchIf(m_ast.begin(), m_ast.end(), eqOp, [](AST::AST::iterator itr)
	{
		auto builtinExpr = Util::NodeCast<BuiltinExpr>(itr.shared_ptr());
		auto declRefName = builtinExpr->FuncDeclRef()->Identifier();

		BUILTIN_ROUTINE(sizeof);
		BUILTIN_ROUTINE(static_assert);

		// NOTE: define any static buildin functions here.
	});

	this->CompletePhase(ConditionTracker::STATIC_RESOLVED);
}

void Semer::FuncToSymbol(std::function<void(const std::string, const std::shared_ptr<ASTNode>& node)> insert)
{
	Compare::Equal<FunctionDecl> eqOp;
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
void Semer::NamedDeclaration()
{
	RESERVE_BUILTIN_ROUTINE("sizeof");
	RESERVE_BUILTIN_ROUTINE("static_assert");

	//FUTURE: Hook in known indentifiers and show warning if they are not defined.

	Compare::Equal<TranslationUnitDecl> traunOp;
	Compare::Derived<Decl> drivdOp;
	MatchIf(m_ast.begin(), m_ast.end(), drivdOp, [&traunOp, this](AST::AST::iterator itr)
	{
		auto node = itr.shared_ptr();
		auto decl = Util::NodeCast<Decl>(node);

		// Ignore translation unit declaration.
		if (traunOp(PTR_NATIVE(decl))) {
			return;
		}

		if (!decl->Identifier().empty()) {
			auto func = Closest<FunctionDecl>(node);
			if (!func) {
				auto block = Closest<CompoundStmt>(node);
				if (!block) {
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

void Semer::ResolveIdentifier()
{
	Compare::Equal<DeclRefExpr> eqOp;
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

					// Internal identifiers are empty.
					if (!binder->second) {
						return;
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

				// Internal identifiers are empty.
				if (!binder->second) {
					return;
				}

				decl->Resolve(binder->second);
				Util::NodeCast<Decl>(binder->second)->RegisterCaller();
			}
		}
	});
}

void Semer::BindPrototype()
{
	Compare::Equal<FunctionDecl> eqOp;
	Stash<ASTNode> m_resolFuncProto;

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
void Semer::DeduceTypes()
{
	// Set signature in function definition.
	Compare::Equal<FunctionDecl> eqOp;
	Compare::Equal<VariadicDecl> eqVaria;
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
	Compare::Equal<CallExpr> eqCall;
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

	// Set return type on list initializers.
	Compare::Equal<InitListExpr> eqList;
	MatchIf(m_ast.begin(), m_ast.end(), eqList, [](AST::AST::iterator itr)
	{
		auto list = Util::NodeCast<InitListExpr>(itr.shared_ptr());
		Typedef::TypeFacade listType;

		// Set list type based on first item type.
		if (!list->List().empty()) {
			const auto firstItemNode = list->List()[0];
			listType = Util::NodeCast<Returnable>(firstItemNode)->ReturnType();
			listType.SetPointer(listType.PointerCount() + 1);
		}
		// Set list type based on parent type.
		else {
			if (const auto parent = Util::NodeCast<Returnable>(list->Parent())) {
				listType = parent->ReturnType();
			}
		}

		list->SetReturnType(listType);
		assert(list->HasReturnType());
	});

	// Set return type on array accessor.
	Compare::Equal<ArraySubscriptExpr> eqArr;
	MatchIf(m_ast.begin(), m_ast.end(), eqArr, [](AST::AST::iterator itr)
	{
		auto subscr = Util::NodeCast<ArraySubscriptExpr>(itr.shared_ptr());
		assert(subscr->ArrayDeclaration()->IsResolved());
		assert(subscr->ArrayDeclaration()->HasReturnType());

		if (!subscr->HasReturnType()) {
			subscr->SetReturnType(subscr->ArrayDeclaration()->Reference()->ReturnType());
		}

		assert(subscr->HasReturnType());
	});

	// Set return type on enum declaration variable.
	Compare::Derived<EnumConstantDecl> enumOp;
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
	Compare::Derived<Operator> drvOp;
	MatchIf(m_ast.begin(), m_ast.end(), drvOp, [](AST::AST::iterator itr)
	{
		auto opr = Util::NodeCast<Operator>(itr.shared_ptr());

		AST delegate{ opr };
		Compare::Derived<Returnable> drvRet;
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
	Compare::Equal<ParenExpr> eqExpr;
	MatchIf(m_ast.begin(), m_ast.end(), eqExpr, [](AST::AST::iterator itr)
	{
		auto paren = Util::NodeCast<ParenExpr>(itr.shared_ptr());

		AST delegate{ paren };
		Compare::Derived<Returnable> drvRet;
		MatchIf(delegate.begin(), delegate.end(), drvRet, [&paren](AST::AST::iterator del_itr)
		{
			auto retType = Util::NodeCast<Returnable>(del_itr.shared_ptr());
			if (retType->HasReturnType() && !paren->HasReturnType()) {
				paren->SetReturnType(retType->ReturnType());
			}
		});

		assert(paren->HasReturnType());
	});

	// Inject a type converter for explicit cast.
	Compare::Equal<CastExpr> eqCast;
	MatchIf(m_ast.begin(), m_ast.end(), eqCast, [](AST::AST::iterator itr)
	{
		auto cast = Util::NodeCast<CastExpr>(itr.shared_ptr());
		SetConversion(cast, cast->Expression());
	});

	//TODO: remove RecordDecl from tree after conversion.
	// Convert record declaration into record type and set the type as return type.
	Compare::Equal<RecordDecl> eqRec;
	MatchIf(m_ast.begin(), m_ast.end(), eqRec, [this](AST::AST::iterator itr)
	{
		auto recordDecl = Util::NodeCast<RecordDecl>(itr.shared_ptr());
		auto recordType = Util::MakeRecordType(recordDecl->Identifier(), recordDecl->Type() == RecordDecl::RecordType::STRUCT
			? Typedef::RecordType::Specifier::STRUCT
			: Typedef::RecordType::Specifier::UNION);

		for (const auto& field : recordDecl->Fields()) {
			recordType->AddField(field->Identifier(), std::make_shared<Typedef::BaseType2::element_type>(field->ReturnType()));
		}

		// Set the return type on the declaration
		recordDecl->SetReturnType(Typedef::TypeFacade{ recordType });

		//TODO: only current valdecl scope
		//TODO: does not continue loop
		// Set return type on every declaration using the record declaration.
		Compare::Equal<VarDecl> eqVal;
		MatchIf(m_ast.begin(), m_ast.end(), eqVal, [&recordDecl, &recordType](AST::AST::iterator rec_itr)
		{
			auto retType = Util::NodeCast<Returnable>(rec_itr.shared_ptr());

			if (!retType->HasReturnType()) { return; }

			//TODO: dynamic_cast should not be the test to see if this in a record type.
			if (auto recType = dynamic_cast<Typedef::RecordType*>(retType->ReturnType().operator->())) {
				if (!recType->IsAnonymous() && recType->Name() == recordType->Name()) {
					retType->SetReturnType(recordDecl->ReturnType());
				}
			}
		});
	});
}

// Check if all datatypes are convertible and inject type conversions in the tree
// when two types can be casted. This method should only perform readonly operations
// on the tree.
void Semer::CheckDataType()
{
	// Compare function with its prototype, if exist.
	Compare::Equal<FunctionDecl> eqFuncOp;
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
	Compare::Equal<CallExpr> eqCallOp;
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
			: std::vector<std::weak_ptr<ASTNode>>{};

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

	//FUTURE: Check if list item can be converted to item type.
	// Test if all the list items have the same datatype.
	Compare::Equal<InitListExpr> eqList;
	MatchIf(m_ast.begin(), m_ast.end(), eqList, [](AST::AST::iterator itr)
	{
		auto list = Util::NodeCast<InitListExpr>(itr.shared_ptr());
		if (list->List().empty()) { return; }

		Typedef::TypeFacade itemType = Util::NodeCast<Returnable>(list->List()[0])->ReturnType();
		for (const auto& item : list->List()) {
			if (Util::NodeCast<Returnable>(item)->ReturnType() != itemType) {
				throw SemanticException{ "conflicting types for 'x'", 0, 0 };
			}
		}
	});

	//TODO: move to DeduceTypes?
	// Inject type converter in vardecl.
	Compare::Equal<VarDecl> eqVar;
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
	Compare::Derived<BinaryOperator> eqOp;
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
	Compare::Equal<ReturnStmt> eqRet;
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
		Compare::MultiDerive<Operator, Expr, Literal> baseNodeOp;
		if (!baseNodeOp(PTR_NATIVE(intializer))) {
			throw SemanticException{ "expected operator, expression or literal", 0, 0 };
		}

		IsConversionRequired(stmt, intializer);
	});
}

void Semer::IllFormedConstruction()
{
	//
}

Semer& CoilCl::Semer::StandardCompliance()
{
	this->CompletePhase(ConditionTracker::COMPLIANT);
	return (*this);
}

Semer& CoilCl::Semer::PedanticCompliance()
{
	return (*this);
}

} // namespace CoilCl