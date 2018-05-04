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

#define DEFAULT_MAKE_CONTEXT() \
	template<typename ContextType, typename... Args> \
	std::shared_ptr<ContextType> MakeContext(Args&&... args) { \
		return std::make_shared<ContextType>(shared_from_this(), std::forward<Args>(args)...); \
	}

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

enum class tag
{
	GLOBAL,
	UNIT,
	FUNCTION,
	COMPOUND,
};

using Global = std::shared_ptr<GlobalContext>;
using Unit = std::shared_ptr<UnitContext>;
using Compound = std::shared_ptr<CompoundContext>;
using Function = std::shared_ptr<FunctionContext>;

// Make global context from existing context or create a new global context
Global MakeGlobalContext(std::shared_ptr<AbstractContext> ctx = nullptr)
{
	if (ctx) {
		return std::static_pointer_cast<GlobalContext>(ctx);
	}
	return std::make_shared<GlobalContext>();
}

} // namespace Context

struct DeclarationRegistry
{
	//TODO
	//virtual PushVar() = 0;
	//TODO:
	// Copy the original and insert the new value in the context
	//void PushVarAsCopy(const std::string& key, std::shared_ptr<AST::ASTNode>& node);
	//TODO:
	// Link value to the original value definition from the caller
	//void PushVarAsPointer(const std::string&& key, std::shared_ptr<Valuedef::Value>&& value);
	// Find the value by identifier, if not found null should be returned
	virtual std::shared_ptr<Valuedef::Value> LookupIdentifier(const std::string& key) = 0;
	// Test if there are any declarations in the current context
	virtual bool HasLocalObjects() const noexcept
	{
		return !m_localObj.empty();
	}
	// Return number of elements in the context
	virtual size_t LocalObjectCount() const noexcept
	{
		return m_localObj.size();
	}

protected:
	std::map<std::string, std::shared_ptr<Valuedef::Value>> m_localObj;
};

struct SymbolRegistry
{
protected:
	std::map<std::string, std::weak_ptr<AST::ASTNode>> m_symbolTable;
};

class AbstractContext
{
	friend class Evaluator;

public:
	std::shared_ptr<AbstractContext> Parent()
	{
		return m_parentContext;
	}

	template<typename ContextType>
	inline std::shared_ptr<ContextType> ParentAs()
	{
		return std::static_pointer_cast<ContextType>(Parent());
	}

	// Find context based on tag, if the context cannot be found,
	// a nullpointer is returned instead.
	template<typename ContextType>
	std::shared_ptr<ContextType> FindContext(Context::tag tag)
	{
		if (m_tag == tag) {
			return std::static_pointer_cast<ContextType>(this->GetSharedSelf());
		}
		if (!Parent()) { return nullptr; }
		return Parent()->FindContext<ContextType>(tag);
	}

public:
	// Create and position a value in the special space
	template<size_t Position>
	void CreateSpecialVar(std::shared_ptr<CoilCl::Valuedef::Value> value, bool override = false)
	{
		if (!m_specialType[Position] || override) {
			m_specialType[Position] = std::move(value);
			return;
		}

		throw std::logic_error{ "cannot set special value multiple time" };
	}

	// Test if an return value is set
	bool HasReturnValue() const noexcept
	{
		return m_specialType[RETURN_VALUE] != nullptr;
	}

	// Retrieve return value
	auto ReturnValue() const noexcept
	{
		return m_specialType[RETURN_VALUE];
	}

private:
	virtual std::shared_ptr<AbstractContext> GetSharedSelf() = 0;

protected:
	AbstractContext(Context::tag tag)
		: m_tag{ tag }
	{
	}

	template<typename Type>
	AbstractContext(Context::tag tag, std::shared_ptr<Type>&& parent)
		: m_tag{ tag }
		, m_parentContext{ std::move(parent) }
	{
	}

private:
	Context::tag m_tag;

protected:
	std::array<std::shared_ptr<CoilCl::Valuedef::Value>, 1> m_specialType;
	std::shared_ptr<AbstractContext> m_parentContext;
};

// Global context is the context of the entire program with
// the possibility of holding multiple unit contexts.
class GlobalContext
	: public AbstractContext
	, public std::enable_shared_from_this<GlobalContext>
{
	friend class Evaluator;

public:
	inline GlobalContext()
		: AbstractContext{ Context::tag::GLOBAL }
	{
	}

	template<typename ContextType, typename... Args>
	std::shared_ptr<ContextType> MakeContext(Args&&... args)
	{
		auto ctx = std::make_shared<ContextType>(shared_from_this(), std::forward<Args>(args)...);
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

	// Run predicate for each context item
	template<typename Predicate, typename CastAsType = AbstractContext>
	void ForEachContext(Predicate&& pred)
	{
		for (const auto& context : m_contextList) {
			if (auto ptr = context.lock()) {
				pred(std::static_pointer_cast<CastAsType>(context));
			}
			else {
				//TODO: remove from list
			}
		}
	}

private:
	virtual std::shared_ptr<AbstractContext> GetSharedSelf()
	{
		return shared_from_this();
	}

private:
	std::list<std::weak_ptr<AbstractContext>> m_contextList;
};

class UnitContext
	: public AbstractContext
	, public SymbolRegistry
	, public DeclarationRegistry
	, public std::enable_shared_from_this<UnitContext>
{
	friend class Evaluator;

public:
	template<typename ContextType>
	UnitContext(std::shared_ptr<ContextType>&& parent, const std::string& name)
		: AbstractContext{ Context::tag::UNIT, std::move(parent) }
		, m_name{ name }
	{
	}

	DEFAULT_MAKE_CONTEXT(); //TODO: some contexts should be initiated from higher up

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

	//TODO:
	// Copy the original and insert the new value in the context
	//void PushVarAsCopy(const std::string& key, std::shared_ptr<AST::ASTNode>& node);

	//TODO:
	// Link value to the original value definition from the caller
	//void PushVarAsPointer(const std::string&& key, std::shared_ptr<Valuedef::Value>&& value);

	//template<typename KeyType, typename ValueType>
	//void PushVar(KeyType&& key, ValueType&& value)
	//{
	//	//FUTURE: std::remove_cvref
	//	using InternalType = std::remove_cv<std::remove_reference<KeyType>::type>::type;
	//	static_assert(std::is_same<InternalType, std::string>::value ||
	//		std::is_same<InternalType, const char *>::value, "");
	//	m_localObj.emplace(std::forward<KeyType>(key), std::forward<ValueType>(value));
	//}
	void PushVar(std::pair<const std::string, std::shared_ptr<Valuedef::Value>>&& pair)
	{
		// Static declarations must be registered in unit scope
		/*if (Util::IsTypeStatic(pair.second->DataType())) {
			m_localObj.insert(std::move(pair));
		}*/
		// External declarations must be ignored (for now)
		/*if (Util::IsTypeExtern(pair.second->DataType())) {

		}*/

		//ParentAs<GlobalContext>()->PushVar(std::move(pair));
	}

	std::shared_ptr<Valuedef::Value> LookupIdentifier(const std::string& key)
	{
		auto val = m_localObj.find(key);
		if (val == m_localObj.end()) {
			//TODO: search higher up
			//return ParentAs<GlobalContext>()->LookupIdentifier(key);
			return nullptr;
		}

		return val->second;
	}

private:
	virtual std::shared_ptr<AbstractContext> GetSharedSelf()
	{
		return shared_from_this();
	}

private:
	//std::list<std::shared_ptr<AbstractContext>> m_objects;
	std::string m_name;
};

namespace Local
{
namespace Detail
{

template<typename ContextType>
struct MakeContextImpl
{
	std::shared_ptr<AbstractContext> contex;
	template<typename ContextType1, typename ContextType2>
	MakeContextImpl(std::shared_ptr<ContextType1>&& ctx, std::shared_ptr<ContextType2>&&)
		: contex{ std::move(ctx) }
	{
	}

	template<typename... Args>
	auto operator()(Args&&... args)
	{
		return std::make_shared<ContextType>(std::move(contex), std::forward<Args>(args)...);
	}
};

template<>
struct MakeContextImpl<FunctionContext>
{
	std::shared_ptr<AbstractContext> contex;
	template<typename ContextType1, typename ContextType2>
	MakeContextImpl(std::shared_ptr<ContextType1>&&, std::shared_ptr<ContextType2>&& ctx)
		: contex{ std::move(ctx) }
	{
	}

	template<typename... Args>
	auto operator()(Args&&... args)
	{
		return std::make_shared<FunctionContext>(std::move(contex), std::forward<Args>(args)...);
	}
};

} // Detail namespace
} // Local namespace

class FunctionContext
	: public AbstractContext
	, public DeclarationRegistry
	, public std::enable_shared_from_this<FunctionContext>
{
	friend class Evaluator;

public:
	template<typename ContextType>
	FunctionContext(std::shared_ptr<ContextType>&& parent, const std::string& name)
		: AbstractContext{ Context::tag::FUNCTION, std::move(parent) }
		, m_name{ name }
	{
	}

	template<typename ContextType, typename... Args>
	std::shared_ptr<ContextType> MakeContext(Args&&... args)
	{
		using namespace Local::Detail;

		assert(Parent());
		return MakeContextImpl<ContextType>{ shared_from_this(), Parent() }(std::forward<Args>(args)...);
	}

	//TODO:
	// Copy the original and insert the new value in the context
	//void PushVarAsCopy(const std::string& key, std::shared_ptr<AST::ASTNode>& node);

	//TODO:
	// Link value to the original value definition from the caller
	//void PushVarAsPointer(const std::string&& key, std::shared_ptr<Valuedef::Value>&& value);

	template<typename KeyType, typename ValueType>
	void PushVar(KeyType&& key, ValueType&& value)
	{
		//FUTURE: std::remove_cvref
		using InternalType = std::remove_cv<std::remove_reference<KeyType>::type>::type;
		static_assert(std::is_same<InternalType, std::string>::value ||
			std::is_same<InternalType, const char *>::value, "");
		m_localObj.emplace(std::forward<KeyType>(key), std::forward<ValueType>(value));
	}
	void PushVar(std::pair<const std::string, std::shared_ptr<Valuedef::Value>>&& pair)
	{
		m_localObj.insert(std::move(pair));
	}

	std::shared_ptr<Valuedef::Value> LookupIdentifier(const std::string& key)
	{
		auto val = m_localObj.find(key);
		if (val == m_localObj.end()) {
			return ParentAs<UnitContext>()->LookupIdentifier(key);
		}

		return val->second;
	}

private:
	virtual std::shared_ptr<AbstractContext> GetSharedSelf()
	{
		return shared_from_this();
	}

private:
	std::string m_name;
};

class CompoundContext
	: public AbstractContext
	, public DeclarationRegistry
	, public std::enable_shared_from_this<CompoundContext>
{
public:
	template<typename ContextType>
	CompoundContext(std::shared_ptr<ContextType>&& parent)
		: AbstractContext{ Context::tag::COMPOUND, std::move(parent) }
	{
	}

	template<typename ContextType, typename... Args>
	std::shared_ptr<ContextType> MakeContext(Args&&... args)
	{
		using namespace Local::Detail;

		assert(FindContext<UnitContext>(Context::tag::UNIT));
		return MakeContextImpl<ContextType>{ shared_from_this(), FindContext<UnitContext>(Context::tag::UNIT) }(std::forward<Args>(args)...);
	}

	//TODO:
	// Copy the original and insert the new value in the context
	//void PushVarAsCopy(const std::string& key, std::shared_ptr<AST::ASTNode>& node);

	//TODO:
	// Link value to the original value definition from the caller
	//void PushVarAsPointer(const std::string&& key, std::shared_ptr<Valuedef::Value>&& value);

	template<typename KeyType, typename ValueType>
	void PushVar(KeyType&& key, ValueType&& value)
	{
		//FUTURE: std::remove_cvref
		using InternalType = std::remove_cv<std::remove_reference<KeyType>::type>::type;
		static_assert(std::is_same<InternalType, std::string>::value ||
			std::is_same<InternalType, const char *>::value, "");
		m_localObj.emplace(std::forward<KeyType>(key), std::forward<ValueType>(value));
	}

	std::shared_ptr<Valuedef::Value> LookupIdentifier(const std::string& key)
	{
		auto val = m_localObj.find(key);
		if (val == m_localObj.end()) {
			return FindContext<FunctionContext>(Context::tag::FUNCTION)->LookupIdentifier(key);
		}

		return val->second;
	}

private:
	virtual std::shared_ptr<AbstractContext> GetSharedSelf()
	{
		return shared_from_this();
	}
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
	const auto value = ctx->LookupIdentifier("str");
	auto result = puts(value->As<std::string>().c_str());
	ctx->CreateSpecialVar<RETURN_VALUE>(Util::MakeInt(result));
}

void _printf(Context::Function& ctx)
{
	const auto value = ctx->LookupIdentifier("fmt");
	auto result = printf(value->As<std::string>().c_str());
	ctx->CreateSpecialVar<RETURN_VALUE>(Util::MakeInt(result));
}

void _scanf(Context::Function& ctx)
{
	const auto value = ctx->LookupIdentifier("fmt");
	auto result = scanf(value->As<std::string>().c_str());
	ctx->CreateSpecialVar<RETURN_VALUE>(Util::MakeInt(result));
}

const std::array<InternalMethod, 3> g_internalMethod = {
	InternalMethod{ "puts", &_puts/*, {"str", STR}*/ },
	InternalMethod{ "printf", &_printf/*, { "fmt", STR }*/ },
	InternalMethod{ "scanf", &_scanf/*, { "fmt", STR }*/ },
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
	if (it == g_internalMethod.cend()) { return nullptr; }
	return &(*it);
}

} // namespace LocalMethod

class Runnable
{
public:
	class Parameter
	{
	public:
		Parameter(const std::string& identifier, const AST::TypeFacade& type)
			: m_identifier{ identifier }
			, m_type{ type }
		{
		}

		inline const std::string Identifier() const noexcept { return m_identifier; }
		inline bool Empty() const noexcept { return m_identifier.empty(); }
		inline AST::TypeFacade DataType() const noexcept { return m_type; }

	private:
		const std::string m_identifier;
		const AST::TypeFacade m_type;
	};

public:
	Runnable() = default;

	// Construct runnable from function declaration
	Runnable(std::shared_ptr<FunctionDecl>& funcNode)
		: m_functionData{ funcNode }
	{
		assert(funcNode);
		assert(funcNode->ParameterStatement()->ChildrenCount());

		for (const auto& child : funcNode->ParameterStatement()->Children()) {
			auto paramDecl = std::static_pointer_cast<ParamDecl>(child.lock());
			assert(paramDecl->HasReturnType());
			m_paramList.push_back(Parameter{ paramDecl->Identifier(), paramDecl->ReturnType() });
		}
	}

	// Construct runnable from internal method
	Runnable(const LocalMethod::InternalMethod *exfuncRef)
		: m_isExternal{ true }
		, m_functionData{ exfuncRef }
	{
		//TODO: Convert args to params
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

	operator bool() const { return m_functionData.empty(); }

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
				for (const auto& child : ptr->Children()) {
					auto node = std::static_pointer_cast<VarDecl>(child.lock());
					if (node->HasExpression()) {
						//TODO: unit must be literal node, otherwise:
						//THROW: initializer element is not constant
						if (Util::IsNodeLiteral(node->Expression())) {
							auto type = std::static_pointer_cast<Literal>(node->Expression())->Type2();
							m_unitContext->PushVar({ node->Identifier(), type });
						}
						else {
							throw std::logic_error{ "initializer element is not constant" };//TODO
						}
					}
				}
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

			{
				//
				// Arithmetic operations
				//
			}

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

			{
				//
				// Bitwise operations
				//
			}

			//TODO: missing bit_or?

		case BinaryOperator::BinOperand::XOR:
			return std::bit_xor<Type>()(left, right);
		case BinaryOperator::BinOperand::AND:
			return std::bit_and<Type>()(left, right);
			/*case BinaryOperator::BinOperand::SLEFT:
				return "<<";
			case BinaryOperator::BinOperand::SRIGHT:
				return ">>";*/

			{
				//
				// Comparisons
				//
			}

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

			{
				//
				// Logical operations
				//
			}

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

	template<typename ContextType>
	static std::shared_ptr<CoilCl::Valuedef::Value> ResolveExpression(std::shared_ptr<AST::ASTNode> node, ContextType& ctx)
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

		{
			//
			// Type casting
			//
		}

		case AST::NodeID::IMPLICIT_CONVERTION_EXPR_ID: {
			auto convRef = std::dynamic_pointer_cast<ImplicitConvertionExpr>(node);
			CRY_UNUSED(convRef);
			return Util::MakeVoid();
		}

		default:
			break;
		}

		throw 1; //TODO
	}

	static void CallExpression(const std::shared_ptr<CallExpr>& callNode, Context::Compound& ctx)
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
		if (auto funcNode = ctx->FindContext<UnitContext>(Context::tag::UNIT)->LookupSymbol<FunctionDecl>(functionIdentifier)) {
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
				const auto literalNode = itArgs->lock();
				if (Util::IsNodeLiteral(literalNode)) {
					auto type = std::static_pointer_cast<Literal>(literalNode)->Type2();
					if (function[i].DataType() != type->DataType()) {
						throw 3; //TODO: source.c:0:0: error: cannot convert argument of type 'X' to parameter type 'Y'
					}
					funcCtx->PushVar(function[i].Identifier(), type);
				}
				else {
					throw std::logic_error{ "initializer element is not literal" };//TODO
				}
				//TODO: StringLiteral is not always the case
				//auto literal = std::static_pointer_cast<StringLiteral>(itArgs->lock());
				//if (function[i].DataType() != literal->Type()->DataType()) {
				//	throw 3; //TODO: source.c:0:0: error: cannot convert argument of type 'X' to parameter type 'Y'
				//}
				//auto val = std::shared_ptr<Valuedef::Value>{ literal->Type() };
				//funcCtx->PushVar(function[i].Identifier(), val);
				++itArgs;
				++i;
			}
		}

		// Call the routine with a new functional context. An new instance is created intentionally
		// to restrict context scope, and to allow the compiler to RAII all resources. The context
		// which is provided with this call can be read one more time after completion to extract
		// the result.
		assert(function);
		if (function.IsExternal()) {
			LocalMethod::ExternalRoutine{}(function.Data<const LocalMethod::InternalMethod*>(), funcCtx);
		}
		else {
			ScopedRoutine{}(function.Data<std::shared_ptr<FunctionDecl>>(), funcCtx);
		}
	}

	// Create new compound context
	void CreateCompound(const std::shared_ptr<AST::ASTNode>& node, Context::Compound& ctx)
	{
		auto compNode = std::static_pointer_cast<CompoundStmt>(node);
		auto compCtx = ctx->MakeContext<CompoundContext>();
		ProcessCompound(compNode, compCtx);
	}

	// Call internal function
	void ProcessRoutine(std::shared_ptr<FunctionDecl>& funcNode, Context::Function& ctx)
	{
		assert(funcNode->ChildrenCount());
		if (funcNode->HasParameters()) {
			//TODO: Do something?
		}

		auto compCtx = ctx->MakeContext<CompoundContext>();
		ProcessCompound(const_cast<std::shared_ptr<CompoundStmt>&>(funcNode->FunctionCompound()), compCtx);
	}

	// Run all nodes in the compound
	void ProcessCompound(std::shared_ptr<CompoundStmt>& compoundNode, Context::Compound& ctx)
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
			case NodeID::SWITCH_STMT_ID: {
				//TODO
				break;
			}
			case NodeID::WHILE_STMT_ID: {
				//TODO
				break;
			}
			case NodeID::DO_STMT_ID: {
				//TODO
				break;
			}
			case NodeID::FOR_STMT_ID: {
				//TODO
				break;
			}
			case NodeID::RETURN_STMT_ID: {
				auto node = std::static_pointer_cast<ReturnStmt>(child);
				Context::Function funcCtx = ctx->FindContext<FunctionContext>(Context::tag::FUNCTION);
				assert(funcCtx);
				ProcessReturn(node, funcCtx);
				return;
			}
			default:
				break;
			}
		}
	}

	// Register the declaration in the current context scope
	void ProcessDeclaration(std::shared_ptr<DeclStmt>& declNode, Context::Compound& ctx)
	{
		for (const auto& child : declNode->Children()) {
			auto node = std::static_pointer_cast<VarDecl>(child.lock());
			if (node->HasExpression()) {
				//Context::Function funcCtx = ctx->FindContext<FunctionContext>(Context::tag::FUNCTION);
				ctx->PushVar(node->Identifier(), ResolveExpression(node->Expression(), ctx));
			}
		}
	}

	void ProcessCondition(std::shared_ptr<IfStmt>& node, Context::Compound& ctx)
	{
		//Context::Function funcCtx = ctx->FindContext<FunctionContext>(Context::tag::FUNCTION);
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
void FormatParameters(std::vector<std::string> pivot, Parameters&& params, Context::Function& ctx)
{
	CRY_UNUSED(params);



	/*for (const auto& item : pivot) {
		ctx->PushVar("kaas", );
	}*/
}

} // namespace

// Call program routine from external context
Evaluator& Evaluator::CallRoutine(const std::string& symbol, const ArgumentList& args)
{
	CRY_UNUSED(args);
	auto funcNode = m_unitContext->LookupSymbol<FunctionDecl>(symbol);
	auto funcCtx = m_unitContext->MakeContext<FunctionContext>(funcNode->Identifier());

	/*FormatParameters({ "argc", "argv" }, ConvertToValueDef(std::move(args)), funcCtx, []() {
		argc = params.size();
		argv = params;
	});*/

	funcCtx->PushVar({ "argc", Util::MakeInt(3) });
	ScopedRoutine{}(funcNode, funcCtx);

	// If the function context contained a return value, set the return value as program exit
	// code. When the program is finished, this global return value serves as the exit code.
	// If any other functions are called within the global scope, then the last set return value
	// determines the exit code.
	if (funcCtx->HasReturnValue()) {
		funcCtx->FindContext<GlobalContext>(Context::tag::GLOBAL)->CreateSpecialVar<RETURN_VALUE>(funcCtx->ReturnValue(), true);
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
