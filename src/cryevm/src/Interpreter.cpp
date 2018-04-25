// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Interpreter.h"

//#include "../../coilcl/src/ASTNode.h"

#define ENTRY_SYMBOL "main"

using namespace EVM;

inline bool IsTranslationUnitNode(const AST::ASTNode& node) noexcept
{
	return node.Label() == AST::NodeID::TRANSLATION_UNIT_DECL_ID;
}

Interpreter::Interpreter(Planner& planner)
	: Strategy{ planner }
{
	//
}

// This strategy defines rules which must be met
// in order to be runnable. Specific conditions
// must hold true to run the program via the
// interpreter. This operation does *not* verify
// the correctness of the program, and it is assumed
// that anything passed in is valid by definition.
bool Interpreter::IsRunnable() const noexcept
{
	return (Program()->Condition().IsRunnable()
		&& !Program()->Ast().empty()
		&& Program()->Ast()->Parent().expired()
		&& IsTranslationUnitNode(Program()->Ast().Front())) || true;
}

class Evaluator;

namespace {

class GlobalContext;
class UnitContext;
class CompoundContext;
class FunctionContext;

class Context
{
	friend class Evaluator;

public:
	static std::shared_ptr<GlobalContext> MakeGlobalContext(std::shared_ptr<Context> ctx = nullptr)
	{
		if (ctx) {
			return std::static_pointer_cast<GlobalContext>(ctx);
		}
		return std::make_shared<GlobalContext>();
	}

protected:
	Context() = default;
	Context(const Context *node)
		: m_parentContext{ node }
	{
	}

	const Context *Parent()
	{
		return m_parentContext;
	}

	template<typename ContextType>
	const ContextType *ParentAs()
	{
		return static_cast<const ContextType*>(m_parentContext);
	}

protected:
	const Context * m_parentContext;
};

// Global context is the context of the entire program with
// the possibility of holding multiple unit contexts.
class GlobalContext : public Context
{
	friend class Evaluator;

public:
	/*template<typename ContextType>
	void AddContext(std::shared_ptr<ContextType>&& context)
	{
		m_contextList.push_back(std::move(context));
	}*/

	template<typename ContextType, typename... Args>
	std::shared_ptr<ContextType> MakeContext(Args&&... args)
	{
		return std::make_shared<ContextType>(this, std::forward<Args>(args)...);
	}

private:
	std::list<std::shared_ptr<Context>> m_contextList;
};

class UnitContext : public Context
{
	friend class Evaluator;

public:
	UnitContext(const Context *parent, const std::string& name)
		: Context{ parent }
		, m_name{ name }
	{
	}

	template<typename Object, typename = typename std::enable_if<std::is_base_of<Context, std::decay<Object>::type>::value>::type>
	void RegisterObject(Object&& object)
	{
		m_objects.push_back(std::make_shared<Object>(object));
	}

	template<typename Node>
	void RegisterSymbol(std::string, std::shared_ptr<Node>);

	template<>
	void RegisterSymbol(std::string symbol, std::shared_ptr<FunctionDecl> node)
	{
		/*if (node->IsPrototypeDefinition() && node->IsExternal()) {
			auto ctx = ParentAs<GlobalContext>();
			ctx->RegisterSymbol(std::forward(symbol, node));
			return;
		}*/
		m_symbolTable.insert({ symbol, node });
	}

	template<typename CastNode>
	std::shared_ptr<CastNode> LookupSymbol(const std::string& symbol)
	{
		auto it = m_symbolTable.find(symbol);
		if (it == m_symbolTable.end()) {
			throw 1; //TODO: No matching entry point was found, exit program
		}

		auto node = it->second.lock();
		if (!node) {
			throw 2; //TODO:
		}

		return node;
	}

	template<>
	std::shared_ptr<FunctionDecl> LookupSymbol(const std::string& symbol)
	{
		assert(Parent());
		auto it = m_symbolTable.find(symbol);
		if (it == m_symbolTable.end()) {
			throw 1; //TODO: No matching entry point was found, exit program
		}

		auto node = it->second.lock();
		if (!node) {
			throw 2; //TODO:
		}

		// If only a function prototype was defined, let the parent context do the work
		auto func = std::dynamic_pointer_cast<FunctionDecl>(node);
		/*if (func->IsPrototypeDefinition()) {
			return ParentAs<GlobalContext>()->LookupSymbol<FunctionDecl>(symbol);
		}*/

		return func;
	}

	template<typename ContextType, typename... Args>
	std::shared_ptr<ContextType> MakeContext(Args&&... args)
	{
		return std::make_shared<ContextType>(this, std::forward<Args>(args)...);
	}

private:
	//std::list<std::shared_ptr<Context>> m_objects;
	std::map<std::string, std::weak_ptr<AST::ASTNode>> m_symbolTable;
	std::string m_name;
};

class CompoundContext : public Context
{
public:
	CompoundContext(const Context *parent)
		: Context{ parent }
	{
	}

	template<typename ContextType, typename... Args>
	std::shared_ptr<ContextType> MakeContext(Args&&... args)
	{
		return std::make_shared<ContextType>(this, std::forward<Args>(args)...);
	}
};

class FunctionContext : public Context
{
	friend class Evaluator;

public:
	FunctionContext(const Context *parent, const std::string& name)
		: Context{ parent }
		, m_name{ name }
	{
	}

	template<typename ContextType, typename... Args>
	std::shared_ptr<ContextType> MakeContext(Args&&... args)
	{
		return std::make_shared<ContextType>(this, std::forward<Args>(args)...);
	}

	/*template<typename... Args>
	std::shared_ptr<FunctionContext> MakeContext<FunctionContext>(Args&&... args)
	{
		assert(Parent());
		return std::make_shared<FunctionContext>(Parent(), std::forward<Args>(args)...);
	}*/

private:
	std::list<std::shared_ptr<UnitContext>> m_objects;
	std::string m_name;
};

} // namespace

std::shared_ptr<UnitContext> InitializeContext(AST::AST& ast, std::shared_ptr<GlobalContext>& ctx)
{
	auto func = static_cast<TranslationUnitDecl&>(ast.Front());
	return ctx->MakeContext<UnitContext>(func.Identifier());
}

class Evaluator
{
	void Global(const TranslationUnitDecl&);
	void Routine(const std::shared_ptr<FunctionDecl>&, std::shared_ptr<FunctionContext>&, const ArgumentList&);
	void CallExpression(const std::shared_ptr<AST::ASTNode>&, std::shared_ptr<FunctionContext>&);

public:
	Evaluator(AST::AST&&, std::shared_ptr<GlobalContext>&);
	Evaluator(AST::AST&&, std::shared_ptr<GlobalContext>&&);

	Evaluator& CallRoutine(const std::string&, const ArgumentList&);
	int YieldResult();

	static int CallFunction(AST::AST&&, const std::string&, const ArgumentList&);

private:
	AST::AST&& m_ast;
	std::shared_ptr<UnitContext> m_unitContext;
};

Evaluator::Evaluator(AST::AST&& ast, std::shared_ptr<GlobalContext>&& ctx)
	: Evaluator{ std::move(ast), ctx }
{
}

Evaluator::Evaluator(AST::AST&& ast, std::shared_ptr<GlobalContext>& ctx)
	: m_ast{ std::move(ast) }
	, m_unitContext{ InitializeContext(m_ast, ctx) }
{
	assert(IsTranslationUnitNode(m_ast.Front()));
	Global(static_cast<TranslationUnitDecl&>(m_ast.Front()));
}

// Global scope evaluation an entire unit
void Evaluator::Global(const TranslationUnitDecl& node)
{
	using namespace AST;

	assert(m_unitContext);
	for (const auto& weaknode : node.Children()) {
		if (auto ptr = weaknode.lock()) {
			//TODO: switch can have more elements
			switch (ptr->Label())
			{
			case NodeID::RECORD_DECL_ID: {
				//TODO:
				// - register record & calculate size
				//m_unitContext->RegisterObject();
				break;
			}
			case NodeID::DECL_STMT_ID: {
				break;
			}
			case NodeID::FUNCTION_DECL_ID: {
				auto func = std::dynamic_pointer_cast<FunctionDecl>(ptr);
				m_unitContext->RegisterSymbol(func->Identifier(), func);
				break;
			}
			default:
				throw 1; //TODO: Throw something usefull
			}
		}
	}
}

void Evaluator::CallExpression(const std::shared_ptr<AST::ASTNode>& node, std::shared_ptr<FunctionContext>& ctx)
{
	auto callExpr = std::static_pointer_cast<CallExpr>(node);
	auto body = callExpr->Children();
	assert(body.size());
	auto declRef = std::static_pointer_cast<DeclRefExpr>(body.at(0).lock());
	assert(declRef->IsResolved());

	//{
	//	auto it = m_unitContext->m_symbolTable.find(declRef->Identifier());
	//	if (it == m_unitContext->m_symbolTable.end()) {
	//		throw 1; //TODO: No matching entry point was found, exit program
	//	}

	//	auto func = it->second.lock();
	//	if (!func) {
	//		throw 2; //TODO:
	//	}

	//	auto funcDecl = std::dynamic_pointer_cast<FunctionDecl>(func);
	//	if (funcDecl->IsPrototypeDefinition()) {
	//		throw 3; //TODO: Cannot call on entry point signatures alone
	//	}
	//}

	auto functional = ctx->MakeContext<FunctionContext>(declRef->Identifier());
	/*Routine(std::dynamic_pointer_cast<FunctionDecl>(func), functional, args);*/
}

static bool HasParams(std::vector<std::weak_ptr<AST::ASTNode>>& children, std::shared_ptr<FunctionContext>& ctx, const ArgumentList& args)
{
	assert(!children.empty());
	auto firstNode = children.at(0).lock();
	if (!firstNode) {
		throw 1; //TODO
	}
	if (firstNode->Label() == AST::NodeID::PARAM_STMT_ID) {
		for (const auto& argument : args) {
			//ctx->PushVar(, argument);
		}

		return true;
	}

	return false;
}

// Function scope routine operation
void Evaluator::Routine(const std::shared_ptr<FunctionDecl>& node, std::shared_ptr<FunctionContext>& ctx, const ArgumentList& args)
{
	using namespace AST;
	CRY_UNUSED(ctx);

	auto body = node->Children();
	if (!body.size()) {
		throw 1; //TODO: no compound in function
	}

	// Function as arguments, commit to context
	auto paredParam = HasParams(body, ctx, args);
	if (paredParam && body.size() < 2) {
		throw 1; //TODO: no compound in function
	}
	else if (!paredParam && body.size() < 3) {
		throw 1; //TODO: no compound in function
	}

	//TODO: Not sure about this one
	if (body.at(1).lock()->Label() != NodeID::COMPOUND_STMT_ID) {
		throw 1; //TODO: throw 1?
	}

	body = body.at(1).lock()->Children();
	if (!body.size()) {
		return; //TODO: Check if return type is void
	}

	for (const auto& childNode : body) {
		auto child = childNode.lock();
		switch (child->Label())
		{
		case NodeID::COMPOUND_STMT_ID:
			puts("COMPOUND_STMT_ID");
			break;
		case NodeID::CALL_EXPR_ID:
			CallExpression(child, ctx);
			break;
		case NodeID::DECL_STMT_ID:
			puts("DECL_STMT_ID");
			break;
		case NodeID::IF_STMT_ID:
			puts("IF_STMT_ID");
			break;
		case NodeID::RETURN_STMT_ID:
			puts("RETURN_STMT_ID");
			break;
		default:
			break;
		}
	}
}

// Convert argument list items to value definitions
const ArgumentList& CovnertValueDef(const ArgumentList& args)
{
	return args;
}

// Call program routine from external context
Evaluator& Evaluator::CallRoutine(const std::string& symbol, const ArgumentList& args)
{
	auto funcNode = m_unitContext->LookupSymbol<FunctionDecl>(symbol);
	auto funcCtx = m_unitContext->MakeContext<FunctionContext>(funcNode->Identifier());
	Routine(funcNode, funcCtx, CovnertValueDef(args));

	return (*this);
}

int Evaluator::YieldResult()
{
	return EXIT_SUCCESS; //TODO: for now
}

int Evaluator::CallFunction(AST::AST&& ast, const std::string& symbol, const ArgumentList& args)
{
	return Evaluator{ std::move(ast), Context::MakeGlobalContext() }
		.CallRoutine(symbol, args)
		.YieldResult();
}

void Interpreter::PreliminaryCheck(const std::string& entry)
{
	assert(!Program()->Ast().Empty());
	assert(Program()->HasSymbols());
	if (!Program()->MatchSymbol(entry)) {
		throw 1;
	}
}

std::string Interpreter::EntryPoint(const std::string entry)
{
	if (entry.empty()) {
		return ENTRY_SYMBOL;
	}
	return entry;
}

// Run the program with current strategy
Interpreter::ReturnCode Interpreter::Execute(const std::string entry, const ArgumentList args)
{
	// Check if settings work for this program.
	PreliminaryCheck(entry);

	return Evaluator::CallFunction(Program()->Ast(), entry, args);
}
