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

class UnitContext;
class CompoundContext;
class FunctionContext;

class Context
{
	friend class Evaluator;

protected:
	Context() = default;
	/*Context(const std::shared_ptr<Context>& node)
		: m_parentContext{ node }
	{
	}*/
	Context(const Context *node)
		: m_parentContext{ node }
	{
	}

	virtual const Context *Parent()
	{
		//return m_parentContext.lock();
		return m_parentContext;
	}

protected:
	//std::weak_ptr<Context> m_parentContext;
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

	void RegisterSymbol(std::string symbol, std::shared_ptr<AST::ASTNode> node)
	{
		m_symbolTable.insert({ symbol, node });
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
	void Routine(const std::shared_ptr<FunctionDecl>&, std::shared_ptr<FunctionContext>&);

public:
	Evaluator(AST::AST&&, std::shared_ptr<GlobalContext>&);

	void CallRoutine(const std::string&);

private:
	AST::AST&& m_ast;
	std::shared_ptr<UnitContext> m_unitContext;
};

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

// Function scope routine operation
void Evaluator::Routine(const std::shared_ptr<FunctionDecl>& node, std::shared_ptr<FunctionContext>& ctx)
{
	CRY_UNUSED(node);
	CRY_UNUSED(ctx);
}

void Evaluator::CallRoutine(const std::string& symbol)
{
	auto it = m_unitContext->m_symbolTable.find(symbol);
	if (it == m_unitContext->m_symbolTable.end()) {
		throw 1; //TODO: No matching entry point was found, exit program
	}

	auto func = it->second.lock();
	if (!func) {
		throw 2; //TODO:
	}

	auto funcdecl = std::dynamic_pointer_cast<FunctionDecl>(func);
	if (funcdecl->IsPrototypeDefinition()) {
		throw 3; //TODO: Cannot call on entry point signatures alone
	}

	auto functional = m_unitContext->MakeContext<FunctionContext>(funcdecl->Identifier());
	Routine(std::dynamic_pointer_cast<FunctionDecl>(func), functional);
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

	auto global = std::make_shared<GlobalContext>();
	Evaluator{ std::move(Program()->Ast()), global }.CallRoutine(entry);

	return EXIT_SUCCESS;
}
