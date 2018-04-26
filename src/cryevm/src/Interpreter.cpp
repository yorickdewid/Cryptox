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

struct InternalMethod
{
	const std::string symbol;
	const std::function<void(const char *)> functional;

	InternalMethod(const std::string symbol, std::function<void(const char *)> func)
		: symbol{ symbol }
		, functional{ func }
	{
	}
};

std::array<InternalMethod, 2> g_internalMethod = std::array<InternalMethod, 2>{
	InternalMethod{ "puts", [](const char *s) { puts(s); } },
		InternalMethod{ "printf", [](const char *s) { printf(s); } },
};

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

class AbstractContext;
class GlobalContext;
class UnitContext;
class CompoundContext;
class FunctionContext;

namespace Context
{

using Global = std::shared_ptr<GlobalContext>;
using Unit = std::shared_ptr<UnitContext>;
using Compound = std::shared_ptr<CompoundContext>;
using Function = std::shared_ptr<FunctionContext>;

Global MakeGlobalContext(std::shared_ptr<AbstractContext> ctx = nullptr)
{
	if (ctx) {
		return std::static_pointer_cast<GlobalContext>(ctx);
	}
	return std::make_shared<GlobalContext>();
}

} // namespace Context

class AbstractContext
{
	friend class Evaluator;

public:
	AbstractContext * Parent()
	{
		return m_parentContext;
	}

	template<typename ContextType>
	ContextType *ParentAs()
	{
		return static_cast<ContextType*>(Parent());
	}

protected:
	AbstractContext() = default;
	AbstractContext(AbstractContext *node)
		: m_parentContext{ node }
	{
	}

protected:
	AbstractContext * m_parentContext;
};

// Global context is the context of the entire program with
// the possibility of holding multiple unit contexts.
class GlobalContext : public AbstractContext
{
	friend class Evaluator;

public:
	template<typename ContextType, typename... Args>
	std::shared_ptr<ContextType> MakeContext(Args&&... args)
	{
		auto ctx = std::make_shared<ContextType>(this, std::forward<Args>(args)...);
		m_contextList.push_back(ctx);
		return std::move(ctx);
	}

	template<typename Node>
	void RegisterSymbol(std::string, std::shared_ptr<Node>)
	{
		//TODO: register symbols with the global context
	}

	template<typename CastNode>
	std::shared_ptr<CastNode> LookupSymbol(const std::string& symbol)
	{
		CRY_UNUSED(symbol);
		return nullptr;
	}

private:
	std::list<std::weak_ptr<AbstractContext>> m_contextList;
};

class UnitContext : public AbstractContext
{
	friend class Evaluator;

public:
	UnitContext(AbstractContext *parent, const std::string& name)
		: AbstractContext{ parent }
		, m_name{ name }
	{
	}

	/*template<typename Object, typename = typename std::enable_if<std::is_base_of<Context, std::decay<Object>::type>::value>::type>
	void RegisterObject(Object&& object)
	{
		m_objects.push_back(std::make_shared<Object>(object));
	}*/

	template<typename Node>
	void RegisterSymbol(std::string, std::shared_ptr<Node>);

	template<>
	void RegisterSymbol(std::string symbol, std::shared_ptr<FunctionDecl> node)
	{
		//TODO: Register the symbol with parent context except for statics
		/*if (node->HasStorageSpecifier(STATIC)) {
			auto ctx = ParentAs<GlobalContext>();
			ctx->RegisterSymbol(symbol, node);
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
		if (func->IsPrototypeDefinition()) {
			return ParentAs<GlobalContext>()->LookupSymbol<FunctionDecl>(symbol);
		}

		return func;
	}

	template<typename ContextType, typename... Args>
	std::shared_ptr<ContextType> MakeContext(Args&&... args)
	{
		return std::make_shared<ContextType>(this, std::forward<Args>(args)...);
	}

private:
	//std::list<std::shared_ptr<AbstractContext>> m_objects;
	std::map<std::string, std::weak_ptr<AST::ASTNode>> m_symbolTable;
	std::string m_name;
};

class CompoundContext : public AbstractContext
{
public:
	CompoundContext(AbstractContext *parent)
		: AbstractContext{ parent }
	{
	}

	template<typename ContextType, typename... Args>
	std::shared_ptr<ContextType> MakeContext(Args&&... args)
	{
		return std::make_shared<ContextType>(this, std::forward<Args>(args)...);
	}
};

namespace Local
{
namespace Detail
{

template<typename ContextType>
struct MakeContextImpl
{
	AbstractContext *contex;
	MakeContextImpl(AbstractContext *ctx, AbstractContext *)
		: contex{ ctx }
	{
	}

	template<typename... Args>
	auto operator()(Args&&... args)
	{
		return std::make_shared<ContextType>(contex, std::forward<Args>(args)...);
	}
};

template<>
struct MakeContextImpl<FunctionContext>
{
	AbstractContext *contex;
	MakeContextImpl(AbstractContext *, AbstractContext *ctx)
		: contex{ ctx }
	{
	}

	template<typename... Args>
	auto operator()(Args&&... args)
	{
		return std::make_shared<FunctionContext>(contex, std::forward<Args>(args)...);
	}
};

} // Detail namespace
} // Local namespace

class FunctionContext : public AbstractContext
{
	friend class Evaluator;

public:
	FunctionContext(AbstractContext *parent, const std::string& name)
		: AbstractContext{ parent }
		, m_name{ name }
	{
	}

	template<typename ContextType, typename... Args>
	std::shared_ptr<ContextType> MakeContext(Args&&... args)
	{
		using namespace Local::Detail;

		assert(Parent());
		return MakeContextImpl<ContextType>{ this, Parent() }(std::forward<Args>(args)...);
	}

	// Create and position a value in the temporary space
	virtual void CreateTempVar(std::shared_ptr<CoilCl::Valuedef::Value> value, bool override = false)
	{
		if (!m_returnType || override) {
			m_returnType = std::move(value);
		}
		else {
			//TODO: maybe throw + warning?
		}
	}

	//TODO:
	virtual void PushVarAsCopy(const std::string& key, std::shared_ptr<AST::ASTNode>& node)
	{
		CRY_UNUSED(key);
		CRY_UNUSED(node);
	}

	// Link value to the original value definition from the caller
	virtual void PushVarAsPointer(const std::string& key, std::shared_ptr<AST::ASTNode>& node)
	{
		m_localObj.insert({ key, node });
	}

	virtual void PushVar(const std::string& key, std::shared_ptr<AST::ASTNode>& node)
	{
		PushVarAsPointer(key, node);
	}

	virtual bool HasLocalObjects() const noexcept
	{
		return !m_localObj.empty();
	}

private:
	std::shared_ptr<CoilCl::Valuedef::Value> m_returnType;
	std::map<std::string, std::shared_ptr<AST::ASTNode>> m_localObj;
	std::string m_name;
};

} // namespace

Context::Unit InitializeContext(AST::AST& ast, std::shared_ptr<GlobalContext>& ctx)
{
	auto func = static_cast<TranslationUnitDecl&>(ast.Front());
	return ctx->MakeContext<UnitContext>(func.Identifier());
}

class Evaluator
{
	void Unit(const TranslationUnitDecl&);

public:
	Evaluator(AST::AST&&, std::shared_ptr<GlobalContext>&);
	Evaluator(AST::AST&&, std::shared_ptr<GlobalContext>&&);

	Evaluator& CallRoutine(const std::string&, const ArgumentList&);
	int YieldResult();

	static int CallFunction(AST::AST&&, const std::string&, const ArgumentList&);

private:
	AST::AST&& m_ast;
	Context::Unit m_unitContext;
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
	Unit(static_cast<TranslationUnitDecl&>(m_ast.Front()));
}

// Global scope evaluation an entire unit
void Evaluator::Unit(const TranslationUnitDecl& node)
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

using Parameters = std::vector<std::shared_ptr<CoilCl::Valuedef::Value>>;

class ScopedRoutine
{
	static void CallExpression(const std::shared_ptr<AST::ASTNode>& node, Context::Function& ctx)
	{
		auto callExpr = std::static_pointer_cast<CallExpr>(node);
		auto body = callExpr->Children();
		assert(body.size());
		auto declRef = std::static_pointer_cast<DeclRefExpr>(body.at(0).lock());
		assert(declRef->IsResolved());

		auto funcNode = ctx->ParentAs<UnitContext>()->LookupSymbol<FunctionDecl>(declRef->Identifier());
		if (!funcNode) {
			//RequestInternalMethod(declRef->Identifier());
		}

		// Create a new function contex
		Context::Function funcCtx = ctx->MakeContext<FunctionContext>(funcNode->Identifier());
		assert(!funcCtx->HasLocalObjects());

		// Check if call expression has arguments
		if (body.size() > 1) {
			auto argStmt = std::static_pointer_cast<ArgumentStmt>(body.at(1).lock());
			if (argStmt->ChildrenCount()) {
				auto paramDecls = funcNode->ParameterStatement()->Children();
				if (paramDecls.size() > argStmt->ChildrenCount()) {
					throw 1; //TODO: source.c:0:0: error: too many arguments to function 'funcNode'
				}
				else if (paramDecls.size() < argStmt->ChildrenCount()) {
					throw 2; //TODO: source.c:0:0: error: too few arguments to function 'funcNode'
				}

				// Assign function arguments to parameters
				auto funcArgs = argStmt->Children();
				auto itArgs = funcArgs.cbegin();
				auto itParam = paramDecls.cbegin();
				while (itArgs != funcArgs.cend() || itParam != paramDecls.cend()) {
					auto paramDecl = std::dynamic_pointer_cast<ParamDecl>(itParam->lock());
					if (paramDecl->Identifier().empty()) {
						throw 3; //TODO: source.c:0:0: error: parameter name omitted to function 'funcNode'
					}
					funcCtx->PushVar(paramDecl->Identifier(), itArgs->lock());
					++itArgs;
					++itParam;
				}
			}
		}

		// Call the routine with a new functional context. An new instance is created intentionally
		// to restrict context scope, and to allow the compiler to RAII all resources. The context
		// which is provided with this call can be read one more time after completion to extract
		// the result.
		ScopedRoutine{}(funcNode, funcCtx);
	}

	void ProcessRoutine(std::shared_ptr<FunctionDecl>& node, Context::Function& ctx)
	{
		using namespace AST;

		auto body = node->Children();
		if (!body.size()) {
			throw 1; //TODO: no compound in function
		}

		// Function as arguments, commit to context
		auto firstNode = body.at(0).lock();
		if (!firstNode) {
			throw 1; //TODO
		}
		if (firstNode->Label() == NodeID::PARAM_STMT_ID) {
			if (body.size() < 2) {
				throw 1; //TODO: no compound in function
			}
		}
		else if (body.size() < 3) {
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
				//TODO: need new scope
				break;
			case NodeID::CALL_EXPR_ID:
				CallExpression(child, ctx);
				break;
			case NodeID::DECL_STMT_ID:
				ProcessDeclaration();
				break;
			case NodeID::IF_STMT_ID:
				ProcessCondition();
				break;
			case NodeID::RETURN_STMT_ID:
				ProcessReturn(std::static_pointer_cast<ReturnStmt>(child), ctx);
				break;
			default:
				break;
			}
		}
	}

	void ProcessDeclaration()
	{
		//
	}

	void ProcessCondition()
	{
		//
	}

	void ProcessReturn(std::shared_ptr<ReturnStmt>& node, Context::Function& ctx)
	{
		if (!node->HasExpression()) {
			ctx->CreateTempVar(CoilCl::Util::MakeVoid());
			return;
		}
		
		//TODO: Quick & dirty, never going to work IRL
		auto intLit = std::dynamic_pointer_cast<IntegerLiteral>(node->Expression());
		ctx->CreateTempVar(intLit->Type());
	}

public:
	void operator()(std::shared_ptr<FunctionDecl>& node, std::shared_ptr<FunctionContext>& ctx)
	{
		ProcessRoutine(node, ctx);
	}
};

namespace
{

// Convert user defined argument list items to parameters
Parameters ConvertToValueDef(const ArgumentList&& args)
{
	using namespace CoilCl::Util;

	struct Converter final : public boost::static_visitor<>
	{
		void operator()(int i) const
		{
			m_paramList.push_back(MakeInt(i));
		}
		void operator()(std::string s) const
		{
			m_paramList.push_back(MakeString(s));
		}

		Converter(Parameters& params)
			: m_paramList{ params }
		{
		}

	private:
		Parameters & m_paramList;
	};

	Parameters params;
	Converter conv{ params };
	for (const auto& arg : args) {
		boost::apply_visitor(conv, arg);
	}

	return params;
}

//TODO:
// Warp startup parameters in main routine format
const Parameters MainParameters(Parameters&& params)
{
	CRY_UNUSED(params);

	return {};
}

} // namespace

// Call program routine from external context
Evaluator& Evaluator::CallRoutine(const std::string& symbol, const ArgumentList& args)
{
	CRY_UNUSED(args);
	auto funcNode = m_unitContext->LookupSymbol<FunctionDecl>(symbol);
	auto funcCtx = m_unitContext->MakeContext<FunctionContext>(funcNode->Identifier());

	//MainParameters(ConvertToValueDef(std::move(args)));

	ScopedRoutine{}(funcNode, funcCtx);

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
