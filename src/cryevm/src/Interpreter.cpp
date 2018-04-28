// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Interpreter.h"

//#include "../../coilcl/src/ASTNode.h"

#include <numeric>

//TODO:
// - Stacktrace

#define RETURN_VALUE 0
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

public:
	// Create and position a value in the special space
	template<size_t Position>
	void CreateSpecialVar(std::shared_ptr<CoilCl::Valuedef::Value> value, bool override = false)
	{
		if (!m_specialType[Position] || override) {
			m_specialType[Position] = std::move(value);
		}
		else {
			//TODO: maybe throw + warning?
		}
	}

	bool HasReturnValue() const noexcept
	{
		return m_specialType[RETURN_VALUE] != nullptr;
	}

	auto ReturnValue() const noexcept
	{
		return m_specialType[RETURN_VALUE];
	}

protected:
	AbstractContext() = default;
	AbstractContext(AbstractContext *node)
		: m_parentContext{ node }
	{
	}

protected:
	std::array<std::shared_ptr<CoilCl::Valuedef::Value>, 1> m_specialType;
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

	//TODO:
	/*void PushVarAsCopy(const std::string& key, std::shared_ptr<AST::ASTNode>& node)
	{
		CRY_UNUSED(key);
		CRY_UNUSED(node);
	}*/

	// Link value to the original value definition from the caller
	/*void PushVarAsPointer(const std::string&& key, std::shared_ptr<Valuedef::Value>&& value)
	{
		m_localObj.insert({ std::move(key), std::move(value) });
	}*/

	void PushVar(const std::string& key, std::shared_ptr<Valuedef::Value>& value)
	{
		m_localObj.insert({ key, value });
	}
	void PushVar(const std::string&& key, std::shared_ptr<Valuedef::Value>&& value)
	{
		m_localObj.insert({ std::move(key), std::move(value) });
	}
	void PushVar(std::pair<const std::string, std::shared_ptr<Valuedef::Value>>&& pair)
	{
		m_localObj.insert(std::move(pair));
	}

	std::shared_ptr<Valuedef::Value> LookupIdentifier(const std::string& key)
	{
		auto val = m_localObj.find(key);
		if (val == m_localObj.end()) {
			//TODO: search higher up
			return nullptr;
		}

		return val->second;
	}

	bool HasLocalObjects() const noexcept
	{
		return !m_localObj.empty();
	}

private:
	std::map<std::string, std::shared_ptr<Valuedef::Value>> m_localObj;
	std::string m_name;
};

} // namespace

  // FUTURE:
  // Local methods must be replaced by the external modules to load functions. Fow now
  // we trust on these few functions, although it is temporary and thus not complete.
namespace LocalMethod
{

struct InternalMethod
{
	const std::string symbol;
	const std::function<void(Context::Function&)> functional;

	InternalMethod(const std::string symbol, std::function<void(Context::Function&)> func)
		: symbol{ symbol }
		, functional{ func }
	{
	}
};

void _puts(Context::Function& ctx)
{
	const auto value = ctx->LookupIdentifier("param1");
	auto result = puts(value->As<std::string>().c_str());
	ctx->CreateSpecialVar<RETURN_VALUE>(Util::MakeInt(result));
}

void _printf(Context::Function& ctx)
{
	const auto value = ctx->LookupIdentifier("param1");
	auto result = printf(value->As<std::string>().c_str());
	ctx->CreateSpecialVar<RETURN_VALUE>(Util::MakeInt(result));
}

const std::array<InternalMethod, 2> g_internalMethod = {
	InternalMethod{ "puts", &_puts/*, {"param1", STR}*/ },
	InternalMethod{ "printf", &_printf/*, { "param1", STR }*/ },
};

struct ExternalRoutine
{
	void ProcessRoutine(const InternalMethod *method, Context::Function& ctx)
	{
		method->functional(ctx);
	}

public:
	inline void operator()(const InternalMethod *method, Context::Function& ctx)
	{
		assert(method);
		ProcessRoutine(method, ctx);
	}
};

const InternalMethod *RequestInternalMethod(const std::string& symbol)
{
	auto it = std::find_if(g_internalMethod.cbegin(), g_internalMethod.cend(), [&](const InternalMethod& method) {
		return method.symbol == symbol;
	});
	if (it == g_internalMethod.cend()) {
		return nullptr;
	}
	return &(*it);
}

} // namespace LocalMethod

class Runnable
{
public:
	class Parameter
	{
	public:
		Parameter(const std::string& identifier)
			: m_identifier{ identifier }
		{
		}

		const std::string Identifier() const
		{
			return m_identifier;
		}

		bool Empty() const noexcept { return m_identifier.empty(); }

	private:
		const std::string m_identifier;
	};

public:
	Runnable() = default;
	Runnable(std::shared_ptr<FunctionDecl>& funcNode)
		: m_functionData{ funcNode }
	{
		assert(funcNode);
		assert(funcNode->ParameterStatement()->ChildrenCount());

		for (const auto& child : funcNode->ParameterStatement()->Children()) {
			auto paramDecl = std::static_pointer_cast<ParamDecl>(child.lock());
			m_paramList.push_back(Parameter{ paramDecl->Identifier() });
		}
	}
	Runnable(const LocalMethod::InternalMethod *exfuncRef)
		: m_isExternal{ true }
		, m_functionData{ exfuncRef }
	{
		/*assert(funcNode);
		assert(funcNode->ParameterStatement()->ChildrenCount());

		for (const auto& child : funcNode->ParameterStatement()->Children()) {
			auto paramDecl = std::static_pointer_cast<ParamDecl>(child.lock());
			m_paramList.push_back(Parameter{ paramDecl->Identifier() });
		}*/
	}

	const Parameter& operator[](size_t idx) const
	{
		return m_paramList[idx];
	}

	inline size_t Size() const noexcept { return m_paramList.size(); }
	inline bool HasArguments() const noexcept { return Size() > 0; }
	inline bool IsExternal() const noexcept { return m_isExternal; }

	template<typename CastType>
	auto Data() const { return boost::get<CastType>(m_functionData); }

private:
	bool m_isExternal = false;
	boost::variant<std::shared_ptr<FunctionDecl>, const LocalMethod::InternalMethod*> m_functionData;
	std::vector<Parameter> m_paramList;
};

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

template<typename Type>
struct OperandFactory
{
	using result_type = Type;

	BinaryOperator::BinOperand operand;
	OperandFactory(BinaryOperator::BinOperand operand)
		: operand{ operand }
	{
	}

	constexpr Type operator()(const Type& left, const Type& right) const
	{
		switch (operand) {

			//
			// Arithmetic operations
			//

		case BinaryOperator::BinOperand::PLUS:
			return std::plus<Type>()(left, right);
		case BinaryOperator::BinOperand::MINUS:
			return std::minus<Type>()(left, right);
		case BinaryOperator::BinOperand::MUL:
			return std::multiplies<Type>()(left, right);
		case BinaryOperator::BinOperand::DIV:
			return std::divides<Type>()(left, right);
		case BinaryOperator::BinOperand::MOD:
			return std::modulus<Type>()(left, right);

			/*case BinOperand::ASSGN:
				return "=";*/

				//
				// Bitwise operations
				//

				//TODO: missing bit_or?

		case BinaryOperator::BinOperand::XOR:
			return std::bit_xor<Type>()(left, right);
		case BinaryOperator::BinOperand::AND:
			return std::bit_and<Type>()(left, right);
			/*case BinaryOperator::BinOperand::SLEFT:
				return "<<";
			case BinaryOperator::BinOperand::SRIGHT:
				return ">>";*/

				//
				// Comparisons
				//

		case BinaryOperator::BinOperand::EQ:
			return std::equal_to<Type>()(left, right);
		case BinaryOperator::BinOperand::NEQ:
			return std::not_equal_to<Type>()(left, right);
		case BinaryOperator::BinOperand::LT:
			return std::less<Type>()(left, right);
		case BinaryOperator::BinOperand::GT:
			return std::greater<Type>()(left, right);
		case BinaryOperator::BinOperand::LE:
			return std::less_equal<Type>()(left, right);
		case BinaryOperator::BinOperand::GE:
			return std::greater_equal<Type>()(left, right);

			//
			// Logical operations
			//

			//TODO: missing negate?

		case BinaryOperator::BinOperand::LAND:
			return std::logical_and<Type>()(left, right);
		case BinaryOperator::BinOperand::LOR:
			return std::logical_or<Type>()(left, right);
		}

		throw 0;
	}
};

class ScopedRoutine
{
	template<typename OperandPred, typename ContainerType = std::shared_ptr<CoilCl::Valuedef::Value>>
	static std::shared_ptr<CoilCl::Valuedef::Value> BinaryOperation(OperandPred predicate, ContainerType&& valuesLHS, ContainerType&& valuesRHS)
	{
		OperandPred::result_type result = predicate(
			valuesLHS->As<OperandPred::result_type>(),
			valuesRHS->As<OperandPred::result_type>());

		return Util::MakeInt(result);
	}

	//FUTURE: Context may be broader than Context::Function
	static std::shared_ptr<CoilCl::Valuedef::Value> ResolveExpression(std::shared_ptr<AST::ASTNode> node, Context::Function& ctx)
	{
		switch (node->Label())
		{
			{
				//
				// Return literal types
				//
			}

		case AST::NodeID::CHARACTER_LITERAL_ID: {
			return std::dynamic_pointer_cast<CharacterLiteral>(node)->Type();
		}
		case AST::NodeID::STRING_LITERAL_ID: {
			return std::dynamic_pointer_cast<StringLiteral>(node)->Type();
		}
		case AST::NodeID::INTEGER_LITERAL_ID: {
			return std::dynamic_pointer_cast<IntegerLiteral>(node)->Type();
		}
		case AST::NodeID::FLOAT_LITERAL_ID: {
			return std::dynamic_pointer_cast<FloatingLiteral>(node)->Type();
		}

		{
			//
			// Operators
			//
		}

		case AST::NodeID::BINARY_OPERATOR_ID: {
			auto op = std::dynamic_pointer_cast<BinaryOperator>(node);
			return BinaryOperation(OperandFactory<int>(op->Operand()), ResolveExpression(op->LHS(), ctx), ResolveExpression(op->RHS(), ctx));
		}
		case AST::NodeID::CONDITIONAL_OPERATOR_ID: {
			std::dynamic_pointer_cast<ConditionalOperator>(node);
			break;
		}
		case AST::NodeID::UNARY_OPERATOR_ID: {
			std::dynamic_pointer_cast<AST::UnaryOperator>(node);
			break;
		}
		case AST::NodeID::COMPOUND_ASSIGN_OPERATOR_ID: {
			std::dynamic_pointer_cast<CompoundAssignOperator>(node);
			break;
		}

		{
			//
			// Return routine result
			//
		}

		case AST::NodeID::CALL_EXPR_ID: {
			//TODO: For some reason returning from a call expression
			// is not possible right now due to some semer bug.
			//TODO: CallExpression();
			break;
		}

		{
			//
			// Lookup symbol reference
			//
		}

		case AST::NodeID::DECL_REF_EXPR_ID: {
			auto declRef = std::dynamic_pointer_cast<DeclRefExpr>(node);
			return ctx->LookupIdentifier(declRef->Identifier());
		}

		default:
			break;
		}

		throw 1; //TODO
	}

	static void CallExpression(const std::shared_ptr<CallExpr>& callNode, Context::Function& ctx)
	{
		assert(callNode);
		assert(callNode->ChildrenCount());
		assert(callNode->FunctionReference()->IsResolved());
		const std::string& functionIdentifier = callNode->FunctionReference()->Identifier();
		Runnable function;

		// Create a new function context. Depending on the parent context the new context
		// is a direct hierarchical or a sub-hierarchical child.
		Context::Function funcCtx = ctx->MakeContext<FunctionContext>(functionIdentifier);
		assert(!funcCtx->HasLocalObjects());

		// The symbol is can be found in different places. The interpreter will locate
		// the runnable object according to the following algorithm:
		//   1.) Look for the symbol in the current program assuming it is a local object
		//   2.) Request the symbol as an external routine (internal or external module)
		//   3.) Throw an symbol not found exception halting from further execution
		if (auto funcNode = ctx->ParentAs<UnitContext>()->LookupSymbol<FunctionDecl>(functionIdentifier)) {
			function = std::move(Runnable{ funcNode });
		}
		else if (auto exfuncRef = LocalMethod::RequestInternalMethod(functionIdentifier)) {
			function = std::move(Runnable{ exfuncRef });
		}
		else {
			throw 0; //TODO: symbol not found in internal or external module
		}

		// Check if call expression has arguments, if so assign paramters to the arguments
		// and commit the arguments into the function context.
		if (callNode->HasArguments()) {
			assert(callNode->ArgumentStatement()->ChildrenCount());
			const auto argsDecls = callNode->ArgumentStatement()->Children();

			// Sanity check, should have been done by semer
			if (function.Size() > argsDecls.size()) {
				throw 1; //TODO: source.c:0:0: error: too many arguments to function 'funcNode'
			}
			else if (function.Size() < argsDecls.size()) {
				throw 2; //TODO: source.c:0:0: error: too few arguments to function 'funcNode'
			}

			// Assign function arguments to parameters
			int i = 0;
			auto itArgs = argsDecls.cbegin();
			while (itArgs != argsDecls.cend()) {
				if (function[i].Empty()) {
					throw 3; //TODO: source.c:0:0: error: parameter name omitted to function 'funcNode'
				}
				//TODO: StringLiteral is not always the case
				auto literal = std::static_pointer_cast<StringLiteral>(itArgs->lock());
				auto val = std::shared_ptr<Valuedef::Value>{ literal->Type() };
				funcCtx->PushVar(function[i].Identifier(), val);
				++itArgs;
				++i;
			}
		}

		// Determine whether to call internal or external routine
		if (function.IsExternal()) {
			LocalMethod::ExternalRoutine{}(function.Data<const LocalMethod::InternalMethod*>(), funcCtx);
		}
		else {
			// Call the routine with a new functional context. An new instance is created intentionally
			// to restrict context scope, and to allow the compiler to RAII all resources. The context
			// which is provided with this call can be read one more time after completion to extract
			// the result.
			ScopedRoutine{}(function.Data<std::shared_ptr<FunctionDecl>>(), funcCtx);
		}
	}

	static void CreateCompound(const std::shared_ptr<AST::ASTNode>& node, Context::Function& ctx)
	{
		CRY_UNUSED(node);
		CRY_UNUSED(ctx);
		//
	}

	//TODO: cleanup messy code
	void ProcessRoutine(std::shared_ptr<FunctionDecl>& funcNode, Context::Function& ctx)
	{
		assert(funcNode->ChildrenCount());
		if (funcNode->HasParameters()) {
			//TODO: Do something?
		}

		ProcessCompound(const_cast<std::shared_ptr<CompoundStmt>&>(funcNode->FunctionCompound()), ctx);
	}

	void ProcessCompound(std::shared_ptr<CompoundStmt>& compoundNode, Context::Function& ctx)
	{
		using namespace AST;

		auto body = compoundNode->Children();
		if (!body.size()) {
			return; //TODO: Check if return type is void
		}

		// Process each child node
		for (const auto& childNode : body) {
			auto child = childNode.lock();
			switch (child->Label())
			{
			case NodeID::COMPOUND_STMT_ID: {
				auto node = std::static_pointer_cast<CompoundStmt>(child);
				CreateCompound(node, ctx);
				break;
			}
			case NodeID::CALL_EXPR_ID: {
				auto node = std::static_pointer_cast<CallExpr>(child);
				CallExpression(node, ctx);
				break;
			}
			case NodeID::DECL_STMT_ID: {
				auto node = std::static_pointer_cast<DeclStmt>(child);
				ProcessDeclaration(node, ctx);
				break;
			}
			case NodeID::IF_STMT_ID: {
				auto node = std::static_pointer_cast<IfStmt>(child);
				ProcessCondition(node, ctx);
				break;
			}
			case NodeID::RETURN_STMT_ID: {
				auto node = std::static_pointer_cast<ReturnStmt>(child);
				ProcessReturn(node, ctx);
				break;
			}
			default:
				break;
			}
		}
	}

	void ProcessDeclaration(std::shared_ptr<DeclStmt>& declNode, Context::Function& ctx)
	{
		for (const auto& child : declNode->Children()) {
			auto node = std::static_pointer_cast<VarDecl>(child.lock());
			if (node->HasExpression()) {
				ctx->PushVar(node->Identifier(), ResolveExpression(node->Expression(), ctx));
			}
		}
	}

	void ProcessCondition(std::shared_ptr<IfStmt>& node, Context::Function& ctx)
	{
		auto value = ResolveExpression(node->Expression(), ctx);
		if (Util::EvaluateAsBoolean(value)) {
			if (node->HasTruthCompound()) {
				auto continueNode = node->TruthCompound();
				auto compoundNode = std::static_pointer_cast<CompoundStmt>(continueNode);
				ProcessCompound(compoundNode, ctx);
			}
		}
		// Handle alternative path, if any
		else if (node->HasAltCompound()) {
			node->AltCompound();
			auto continueNode = node->TruthCompound();
			auto compoundNode = std::static_pointer_cast<CompoundStmt>(continueNode);
			ProcessCompound(compoundNode, ctx);
		}
	}

	void ProcessReturn(std::shared_ptr<ReturnStmt>& node, Context::Function& ctx)
	{
		// Create explicit return type
		if (!node->HasExpression()) {
			ctx->CreateSpecialVar<RETURN_VALUE>(CoilCl::Util::MakeVoid());
			return;
		}

		// Resolve return expression
		ctx->CreateSpecialVar<RETURN_VALUE>(ResolveExpression(node->Expression(), ctx));
	}

public:
	inline void operator()(std::shared_ptr<FunctionDecl>& node, std::shared_ptr<FunctionContext>& ctx)
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
// Warp startup parameters in format
const Parameters FormatParameters(std::vector<std::string> pivot, Parameters&& params)
{
	CRY_UNUSED(params);

	/*for (const auto& item : pivot) {
		item
	}*/

	return {};
}

} // namespace

// Call program routine from external context
Evaluator& Evaluator::CallRoutine(const std::string& symbol, const ArgumentList& args)
{
	CRY_UNUSED(args);
	auto funcNode = m_unitContext->LookupSymbol<FunctionDecl>(symbol);
	auto funcCtx = m_unitContext->MakeContext<FunctionContext>(funcNode->Identifier());

	//FormatParameters({ "argc", "argv" }, ConvertToValueDef(std::move(args)));

	funcCtx->PushVar({ "argc", Util::MakeInt(3) });
	ScopedRoutine{}(funcNode, funcCtx);

	// If the function context contained a return value, set the return value as program exit
	// code. When the program is finished, this global return value serves as the exit code.
	// If any other functions are called within the global scope, then the last set return value
	// determines the exit code.
	if (funcCtx->HasReturnValue()) {
		funcCtx->Parent()->ParentAs<GlobalContext>()->CreateSpecialVar<RETURN_VALUE>(funcCtx->ReturnValue(), true);
	}

	return (*this);
}

int Evaluator::YieldResult()
{
	try {
		auto globalCtx = m_unitContext->ParentAs<GlobalContext>();
		if (globalCtx->HasReturnValue()) {
			return globalCtx->ReturnValue()->As<int>();
		}
	}
	// On casting faillure, return faillure all the way
	catch (std::exception&) { //TODO: catch the boost casting error here
		return EXIT_FAILURE;
	}

	// Exit with success by no means of determine the program result
	return EXIT_SUCCESS;
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
