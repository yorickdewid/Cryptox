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
// interpreter.
bool Interpreter::IsRunnable() const noexcept
{
	return (Program()->Condition().IsRunnable()
		&& !Program()->Ast().empty()
		&& Program()->Ast()->Parent().expired()
		&& IsTranslationUnitNode(Program()->Ast().Front())) || true;
}

namespace {

class ProgramContext
{
public:
	virtual void RegisterObject() {}
};

class GlobalContext : public ProgramContext
{
public:
	GlobalContext()
	{
	}

	~GlobalContext()
	{
	}

private:

};

class UnitContext : public GlobalContext
{
public:
	UnitContext(const std::string& name)
		: m_name{ name }
	{
	}

	template<typename Object, typename = typename std::enable_if<std::is_base_of<UnitContext, std::decay<Object>::type>::value>::type>
	void RegisterObject(Object&& object)
	{
		m_objects.push_back(std::make_shared<Object>(object));
	}

	static std::unique_ptr<UnitContext> Make(const std::string& name)
	{
		return std::make_unique<UnitContext>(name);
	}

private:
	std::list<std::shared_ptr<UnitContext>> m_objects;
	std::string m_name;
};

class FunctionContext : public UnitContext
{
public:
	FunctionContext(const std::string& name, bool isProto = false)
		: UnitContext{ "" } //TODO: for now
		, m_name{ name }
		, m_isSignature{ isProto }
	{
	}

private:
	std::string m_name;
	// signature ?
	bool m_isSignature = false;
	bool m_isExternal = false;
};

} // namespace

class Evaluator
{
	void Global(const TranslationUnitDecl&);
	void Routine(const std::shared_ptr<FunctionDecl>&);
	void Expression(const std::shared_ptr<IfStmt>&);

public:
	Evaluator(AST::AST&&);

	void CallFunction(const std::string&);

private:
	std::unique_ptr<UnitContext> m_unitContext;
};

Evaluator::Evaluator(AST::AST&& ast)
{
	assert(IsTranslationUnitNode(ast.Front()));
	Global(static_cast<TranslationUnitDecl&>(ast.Front()));
}

// Global scope
void Evaluator::Global(const TranslationUnitDecl& node)
{
	using namespace AST;

	assert(!m_unitContext);
	m_unitContext = std::move(UnitContext::Make(node.Identifier()));
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
			case NodeID::FUNCTION_DECL_ID: {
				//TODO:
				// - register functions & signatures & if callable
				//Routine(std::dynamic_pointer_cast<FunctionDecl>(ptr));

				auto func = std::dynamic_pointer_cast<FunctionDecl>(ptr);
				m_unitContext->RegisterObject(FunctionContext{ func->Identifier(), func->IsPrototypeDefinition() });
				break;
			}
			default:
				throw 1; //TODO: Throw something usefull
			}
		}
	}
}

// Function scope
void Evaluator::Routine(const std::shared_ptr<FunctionDecl>& /*node*/)
{
	//TODO:
	// - Create function context
}

// Expression resolver
void Evaluator::Expression(const std::shared_ptr<IfStmt>& /*node*/)
{
	//
}

void Evaluator::CallFunction(const std::string&)
{
	//
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
Interpreter::ReturnCode Interpreter::Execute(const std::string entry)
{
	// Check if settings work for this program.
	PreliminaryCheck(entry);

	Evaluator{ std::move(Program()->Ast()) }.CallFunction(entry);

	return EXIT_SUCCESS;
}
