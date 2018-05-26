// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Interpreter.h"

#include "Cry/Except.h"

//#include "../../coilcl/src/ASTNode.h"

#include <numeric>

//TODO:
// - Stacktrace
// - Infinite loop detection
// - Runtime resolve

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

class IdentifierNotFoundException : public std::exception
{
public:
	IdentifierNotFoundException(const std::string& identifier)
		: m_msg{ "identifier '" + identifier + "' undeclared in hierarchical context" }
	{
	}

	virtual const char *what() const noexcept
	{
		return m_msg.c_str();
	}

private:
	std::string m_msg;
};

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
	// Access local storage object directly without hirarchiecal lookup
	virtual void ForEachObject(std::function<void(std::shared_ptr<Valuedef::Value>)> delegate) const
	{
		for (const auto& item : m_localObj) {
			delegate(item.second);
		}
	}

#ifdef CRY_DEBUG
	void DumpVar(const std::string& key)
	{
		auto var = LookupIdentifier(key);
		if (!var) { printf("%s -> (null)", key.c_str()); }
		else {
			printf("%s -> ", key.c_str());
			DUMP_VALUE(var);
		}
	}
#endif

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
	template<typename ToContextType, typename FromContextType>
	static inline std::shared_ptr<ToContextType> CastUpAs(std::shared_ptr<FromContextType>& ctx)
	{
		return std::static_pointer_cast<ToContextType>(ctx);
	}
	template<typename ToContextType, typename FromContextType>
	static inline std::shared_ptr<ToContextType> CastDownAs(std::shared_ptr<FromContextType>& ctx)
	{
		return std::dynamic_pointer_cast<ToContextType>(ctx);
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
			CryImplExcept(); //TODO: No matching entry point was found, exit program
		}

		auto node = it->second.lock();
		if (!node) {
			CryImplExcept(); //TODO:
		}

		return node;
	}

	template<>
	std::shared_ptr<FunctionDecl> LookupSymbol(const std::string& symbol)
	{
		assert(Parent());
		auto it = m_symbolTable.find(symbol);
		if (it == m_symbolTable.end()) {
			CryImplExcept(); //TODO: No matching entry point was found, exit program
		}

		auto node = it->second.lock();
		if (!node) {
			CryImplExcept(); //TODO:
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
		CRY_UNUSED(pair);
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
			throw IdentifierNotFoundException{ key };
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
		using InternalType = typename std::remove_cv<typename std::remove_reference<KeyType>::type>::type;
		static_assert(std::is_same<InternalType, std::string>::value ||
			std::is_same<InternalType, const char *>::value, "");
		m_localObj.emplace(std::forward<KeyType>(key), std::forward<ValueType>(value));
	}
	void PushVar(std::pair<const std::string, std::shared_ptr<Valuedef::Value>>&& pair)
	{
		m_localObj.insert(std::move(pair));
	}

	void AttachCompound(Context::Compound& ctx)
	{
		assert(!m_bodyContext);
		m_bodyContext = ctx;
	}

	std::shared_ptr<Valuedef::Value> LookupIdentifier(const std::string& key)
	{
		auto val = m_localObj.find(key);
		if (val == m_localObj.end()) {
			auto compoundVal = CastDownAs<DeclarationRegistry>(m_bodyContext)->LookupIdentifier(key);
			if (compoundVal) {
				return compoundVal;
			}
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
	Context::Compound m_bodyContext;
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
		using InternalType = typename std::remove_cv<typename std::remove_reference<KeyType>::type>::type;
		static_assert(std::is_same<InternalType, std::string>::value ||
			std::is_same<InternalType, const char *>::value, "");
		m_localObj.emplace(std::forward<KeyType>(key), std::forward<ValueType>(value));
	}

	std::shared_ptr<Valuedef::Value> LookupIdentifier(const std::string& key)
	{
		auto val = m_localObj.find(key);
		if (val == m_localObj.end()) {
			return std::dynamic_pointer_cast<DeclarationRegistry>(Parent())->LookupIdentifier(key);
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

class SolidParameterFormat
{
	std::shared_ptr<ParamStmt> stmt{ AST::MakeASTNode<ParamStmt>() };

public:
	std::shared_ptr<ParamStmt> Parameters() const { return stmt; };

	SolidParameterFormat& Parse(const char str[])
	{
		for (size_t i = 0; i < strlen(str); ++i) {
			assert(i < 100);
			switch (str[i]) {
			case 'i': {
				auto decl = AST::MakeASTNode<ParamDecl>(Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::INT));
				stmt->AppendParamter(decl);
				break;
			}
			case 's': {
				auto decl = AST::MakeASTNode<ParamDecl>(Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::CHAR));
				stmt->AppendParamter(decl);
				break;
			}
			case 'f': {
				auto decl = AST::MakeASTNode<ParamDecl>(Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::FLOAT));
				stmt->AppendParamter(decl);
				break;
			}
			case 'b': {
				auto decl = AST::MakeASTNode<ParamDecl>(Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::BOOL));
				stmt->AppendParamter(decl);
				break;
			}
			case 'd': {
				auto decl = AST::MakeASTNode<ParamDecl>(Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::DOUBLE));
				stmt->AppendParamter(decl);
				break;
			}
			case 'L': {
				auto decl = AST::MakeASTNode<ParamDecl>(Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::LONG));
				stmt->AppendParamter(decl);
				break;
			}
			case 'S': {
				auto decl = AST::MakeASTNode<ParamDecl>(Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::SHORT));
				stmt->AppendParamter(decl);
				break;
			}
			case 'U': {
				auto decl = AST::MakeASTNode<ParamDecl>(Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::UNSIGNED));
				stmt->AppendParamter(decl);
				break;
			}
			case 'G': {
				auto decl = AST::MakeASTNode<ParamDecl>(Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::SIGNED));
				stmt->AppendParamter(decl);
				break;
			}
			case 'v': {
				auto decl = AST::MakeASTNode<ParamDecl>(Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::VOID_T));
				stmt->AppendParamter(decl);
				break;
			}
			case 'V': {
				auto decl = AST::MakeASTNode<VariadicDecl>();
				stmt->AppendParamter(decl);
				break;
			}
			}
		}

		return (*this);
	}
};

#define PACKED_PARAM_DECL(s) SolidParameterFormat{}.Parse(s).Parameters()

struct InternalMethod
{
	const std::string symbol;
	const std::function<void(Context::Function&)> functional;
	const std::shared_ptr<ParamStmt> params;

	bool HasParameters() const noexcept { return params != nullptr; }

	InternalMethod(const std::string symbol, std::function<void(Context::Function&)> func, std::shared_ptr<ParamStmt> params = {})
		: symbol{ symbol }
		, functional{ func }
		, params{ params }
	{
	}
};

#define NATIVE_WRAPPER(c) void _##c(Context::Function& ctx)
#define GET_DEFAULT_ARG(i) ctx->LookupIdentifier("__arg" #i "__")
#define GET_VA_LIST_ARG(i) ctx->LookupIdentifier("__va_arg" #i "__")

NATIVE_WRAPPER(puts)
{
	const auto value = GET_DEFAULT_ARG(0);
	assert(value);
	const auto arg0 = value->As<std::string>();
	auto result = puts(arg0.c_str());
	ctx->CreateSpecialVar<RETURN_VALUE>(Util::MakeInt(result));
}

NATIVE_WRAPPER(putchar)
{
	const auto value = GET_DEFAULT_ARG(0);
	assert(value);
	const auto arg0 = value->As<int>();
	auto result = putchar(arg0);
	ctx->CreateSpecialVar<RETURN_VALUE>(Util::MakeInt(result));
}

//TODO: create full string format wrapper
NATIVE_WRAPPER(printf)
{
	const auto value = GET_DEFAULT_ARG(0);
	const auto value2 = GET_VA_LIST_ARG(0);
	assert(value);
	assert(value2);
	const auto arg0 = value->As<std::string>();
	const auto arg1 = value2->As<int>();
	auto result = printf(arg0.c_str(), arg1);
	ctx->CreateSpecialVar<RETURN_VALUE>(Util::MakeInt(result));
}

NATIVE_WRAPPER(scanf)
{
	const auto value = GET_DEFAULT_ARG(0);
	assert(value);
	const auto arg0 = value->As<std::string>();
	auto result = scanf(arg0.c_str());
	ctx->CreateSpecialVar<RETURN_VALUE>(Util::MakeInt(result));
}

NATIVE_WRAPPER(error)
{
	const auto value = GET_DEFAULT_ARG(0);
	const auto value2 = GET_VA_LIST_ARG(0);
	assert(value);
	assert(value2);
	const auto arg0 = value->As<int>();
	const auto arg1 = value->As<std::string>();
	throw arg0; //TODO: or something
}

const std::array<InternalMethod, 5> g_internalMethod = {
	InternalMethod{ "puts", &_puts, PACKED_PARAM_DECL("s") },
	InternalMethod{ "putchar", &_putchar, PACKED_PARAM_DECL("i") },
	InternalMethod{ "printf", &_printf, PACKED_PARAM_DECL("sV") },
	InternalMethod{ "scanf", &_scanf, PACKED_PARAM_DECL("sV") },
	InternalMethod{ "error", &_error, PACKED_PARAM_DECL("is") },
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

		explicit Parameter(const std::string& identifier)
			: m_identifier{ identifier }
			, m_isVariadic{ true }
		{
		}

		inline const std::string Identifier() const noexcept { return m_identifier; }
		inline bool Empty() const noexcept { return m_identifier.empty(); }
		inline AST::TypeFacade DataType() const noexcept { return m_type; }
		inline bool IsVariadic() const noexcept { return m_isVariadic; }

	private:
		const bool m_isVariadic{ false }; //TODO: incorporate in datatype
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

		if (!funcNode->HasParameters()) { return; }
		for (const auto& child : funcNode->ParameterStatement()->Children()) {
			if (child.lock()->Label() == AST::NodeID::VARIADIC_DECL_ID) {
				std::string autoArg{ "__va_list__" };
				break;
				m_paramList.push_back(Parameter{ autoArg });
			}
			else {
				auto paramDecl = Util::NodeCast<ParamDecl>(child);
				assert(paramDecl->HasReturnType());
				m_paramList.push_back(Parameter{ paramDecl->Identifier(), paramDecl->ReturnType() });
			}
		}
	}

	// Construct runnable from internal method
	Runnable(const LocalMethod::InternalMethod *exfuncRef)
		: m_isExternal{ true }
		, m_functionData{ exfuncRef }
	{
		assert(exfuncRef);

		int counter = 0;
		if (!exfuncRef->HasParameters()) { return; }
		for (const auto& child : exfuncRef->params->Children()) {
			if (child.lock()->Label() == AST::NodeID::VARIADIC_DECL_ID) {
				std::string autoArg{ "__va_list__" };
				m_paramList.push_back(Parameter{ autoArg });
				break;
			}
			else {
				auto paramDecl = Util::NodeCast<ParamDecl>(child);
				assert(paramDecl->HasReturnType());
				std::string autoArg{ "__arg" + std::to_string(counter++) + "__" };
				m_paramList.push_back(Parameter{ autoArg, paramDecl->ReturnType() });
			}
		}
	}

	//TODO: confusing call
	const Parameter& operator[](size_t idx) const
	{
		return m_paramList[idx];
	}

	const Parameter& Front() const { return m_paramList.front(); }
	const Parameter& Back() const { return m_paramList.back(); }

	// Query argument size
	inline size_t ArgumentSize() const noexcept { return m_paramList.size(); }
	// Query if runnable has arguments
	inline bool HasArguments() const noexcept { return ArgumentSize() > 0; }
	// Query if runnable is external method
	inline bool IsExternal() const noexcept { return m_isExternal; }

	template<typename CastType>
	auto Data() const { return boost::get<CastType>(m_functionData); }

	operator bool() const { return !m_functionData.empty(); }

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
			case NodeID::TYPEDEF_DECL_ID: {
				// Type definitions should already have been resolved in an earlier state
				// and thus is error checking alone sufficient.
				auto typedefDecl = Util::NodeCast<TypedefDecl>(ptr);
				if (!typedefDecl->HasReturnType()) {
					throw std::logic_error{ "type alias is empty" };//TODO
				}
				break;
			}
			case NodeID::DECL_STMT_ID: {
				for (const auto& child : ptr->Children()) {
					auto varDecl = Util::NodeCast<VarDecl>(child);
					if (varDecl->HasExpression()) {
						if (Util::IsNodeLiteral(varDecl->Expression())) {
							auto type = Util::NodeCast<Literal>(varDecl->Expression())->Type2();
							m_unitContext->PushVar({ varDecl->Identifier(), type });
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
				CryImplExcept(); //TODO: Throw something usefull
			}
		}
	}
}

using Parameters = std::vector<std::shared_ptr<CoilCl::Valuedef::Value>>;

template<typename Type>
struct BitLeftShift
{
	constexpr Type operator()(const Type& left, const Type& right) const
	{
		return (left << right);
	}
};

template<typename Type>
struct BitRightShift
{
	constexpr Type operator()(const Type& left, const Type& right) const
	{
		return (left >> right);
	}
};

template<typename Type>
struct OperandFactory
{
	using result_type = Type;

	BinaryOperator::BinOperand operand;
	inline OperandFactory(BinaryOperator::BinOperand operand)
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
		case BinaryOperator::BinOperand::SLEFT:
			return BitLeftShift<Type>()(left, right);
		case BinaryOperator::BinOperand::SRIGHT:
			return BitRightShift<Type>()(left, right);

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

		CryImplExcept();
	}
};

class ScopedRoutine
{
	template<typename OperandPred, typename ContainerType = std::shared_ptr<CoilCl::Valuedef::Value>>
	static std::shared_ptr<CoilCl::Valuedef::Value> BinaryOperation(OperandPred predicate, ContainerType&& valuesLHS, ContainerType&& valuesRHS)
	{
		typename OperandPred::result_type result = predicate(
			valuesLHS->As<OperandPred::result_type>(),
			valuesRHS->As<OperandPred::result_type>());

		return Util::MakeInt(result); //TODO: not always an integer
	}

	static void AssignmentOperation(std::shared_ptr<CoilCl::Valuedef::Value> assign, std::shared_ptr<CoilCl::Valuedef::Value> value)
	{
		assign->ReplaceValueWithValue(*(value.get()));
	}

	static std::shared_ptr<CoilCl::Valuedef::Value> EvaluateInverse(std::shared_ptr<CoilCl::Valuedef::Value>&& value)
	{
		return Util::MakeBool(!Util::EvaluateAsBoolean(value));
	}

	//FUTURE: both operations can be improved
	template<typename OperandPred>
	static std::shared_ptr<CoilCl::Valuedef::Value> ValueAlteration(OperandPred predicate, AST::UnaryOperator::OperandSide side, std::shared_ptr<CoilCl::Valuedef::Value>&& value)
	{
		int result = predicate(value->As<int>(), 1);

		// On postfix operand, copy the original first
		if (side == AST::UnaryOperator::OperandSide::POSTFIX) {
			auto newval = Util::MakeInt(value->As<int>());
			value->ReplaceValue(result);
			return newval;
		}

		// On prefix, perform the unary operand on the original
		value->ReplaceValue(result);
		return value;
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
			if (op->Operand() == BinaryOperator::BinOperand::ASSGN) {
				auto assignValue = ResolveExpression(op->LHS(), ctx);
				AssignmentOperation(assignValue, ResolveExpression(op->RHS(), ctx));
				return assignValue;
			}
			return BinaryOperation(OperandFactory<int>(op->Operand()), ResolveExpression(op->LHS(), ctx), ResolveExpression(op->RHS(), ctx));
		}
		case AST::NodeID::CONDITIONAL_OPERATOR_ID: {
			auto op = std::dynamic_pointer_cast<ConditionalOperator>(node);
			auto value = ResolveExpression(op->Expression(), ctx);
			if (Util::EvaluateAsBoolean(value)) {
				return ResolveExpression(op->TruthStatement(), ctx);
			}

			// Handle alternative path
			return ResolveExpression(op->AltStatement(), ctx);
		}
		case AST::NodeID::UNARY_OPERATOR_ID: {
			auto op = std::dynamic_pointer_cast<AST::UnaryOperator>(node);
			auto value = ResolveExpression(op->Expression(), ctx);
			switch (op->Operand())
			{
			case AST::UnaryOperator::UnaryOperand::INC:
				return ValueAlteration(std::plus<int>(), op->OperationSide(), std::move(value));
			case AST::UnaryOperator::UnaryOperand::DEC:
				return ValueAlteration(std::minus<int>(), op->OperationSide(), std::move(value));

			case AST::UnaryOperator::UnaryOperand::INTPOS:
			case AST::UnaryOperator::UnaryOperand::INTNEG:
				//ValueSignedness();
				break;

			case AST::UnaryOperator::UnaryOperand::ADDR:
				//ValueAddress();
				break;
			case AST::UnaryOperator::UnaryOperand::PTRVAL:
				//ValueIndirection();
				break;

				//case AST::UnaryOperator::UnaryOperand::BITNOT:
			case AST::UnaryOperator::UnaryOperand::BOOLNOT:
				return EvaluateInverse(std::move(value));
			}
			CryImplExcept(); //TODO:
		}
		case AST::NodeID::COMPOUND_ASSIGN_OPERATOR_ID: {
			Util::NodeCast<CompoundAssignOperator>(node);
			break;
		}

		{
			//
			// Return routine result
			//
		}

		case AST::NodeID::CALL_EXPR_ID: {
			auto op = Util::NodeCast<CallExpr>(node);
			Context::Function funcCtx = CallExpression(op, ctx);
			if (!funcCtx->HasReturnValue()) {
				throw std::logic_error{ "function must return a value" }; //TODO:
			}
			return funcCtx->ReturnValue();
		}

		{
			//
			// Lookup symbol reference
			//
		}

		case AST::NodeID::DECL_REF_EXPR_ID: {
			auto declRef = Util::NodeCast<DeclRefExpr>(node);
			return ctx->LookupIdentifier(declRef->Identifier());
		}

		{
			//
			// Type casting
			//
		}

		case AST::NodeID::IMPLICIT_CONVERTION_EXPR_ID: {
			auto convRef = Util::NodeCast<ImplicitConvertionExpr>(node);
			CRY_UNUSED(convRef);
			return Util::MakeVoid(); //TODO: for now
		}

		default:
			break;
		}

		CryImplExcept(); //TODO
	}

	template<typename ContextType>
	static Context::Function CallExpression(const std::shared_ptr<CallExpr>& callNode, ContextType& ctx)
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
			CryImplExcept(); //TODO: symbol not found in internal or external module
		}

		// Check if call expression has arguments, if so assign paramters to the arguments
		// and commit the arguments into the function context.
		if (callNode->HasArguments() && function.HasArguments()) {
			assert(callNode->ArgumentStatement()->ChildrenCount());
			const auto argsDecls = callNode->ArgumentStatement()->Children();

			// Sanity check, should have been done by semer
			if (function.ArgumentSize() > argsDecls.size()) {
				CryImplExcept(); //TODO: source.c:0:0: error: too many arguments to function 'funcNode'
			}
			else if (function.ArgumentSize() < argsDecls.size() && !function.Back().IsVariadic()) {
				CryImplExcept(); //TODO: source.c:0:0: error: too few arguments to function 'funcNode'
			}

			// Assign function arguments to parameters
			int i = 0;
			auto itArgs = argsDecls.cbegin();
			while (itArgs != argsDecls.cend()) {
				if (function[i].Empty()) {
					CryImplExcept(); //TODO: source.c:0:0: error: parameter name omitted to function 'funcNode'
				}
				if (function[i].IsVariadic()) {
					int v_i = 0;
					while (itArgs != argsDecls.cend()) {
						const auto value = ResolveExpression(itArgs->lock(), ctx);
						std::string autoVA{ "__va_arg" + std::to_string(v_i++) + "__" };
						funcCtx->PushVar(autoVA, Util::ValueCopy(value));
						++itArgs;
					}
					break;
				}
				else {
					const auto value = ResolveExpression(itArgs->lock(), ctx);
					if (function[i].DataType() != value->DataType()) {
						CryImplExcept(); //TODO: source.c:0:0: error: cannot convert argument of type 'X' to parameter type 'Y'
					}
					//TODO: check if param is pointer
					funcCtx->PushVar(function[i].Identifier(), Util::ValueCopy(value));
					++itArgs;
					++i;
				}
			}
		}

		// Call the routine with a new functional context. An new instance is created intentionally
		// to restrict context scope, and to allow the compiler to RAII all resources. The context
		// is returned as the result of the expression.
		assert(function);
		if (function.IsExternal()) {
			LocalMethod::ExternalRoutine{}(function.Data<const LocalMethod::InternalMethod*>(), funcCtx);
		}
		else {
			auto func = function.Data<std::shared_ptr<FunctionDecl>>();
			ScopedRoutine{}(func, funcCtx);
		}

		return funcCtx;
	}

	// Create new compound context
	void CreateCompound(const std::shared_ptr<AST::ASTNode>& node, Context::Compound& ctx)
	{
		auto compNode = Util::NodeCast<CompoundStmt>(node);
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
		ctx->AttachCompound(compCtx);
		ProcessCompound(const_cast<std::shared_ptr<CompoundStmt>&>(funcNode->FunctionCompound()), compCtx);
	}

	// Run all nodes in the compound
	template<typename Node>
	void ProcessCompound(const std::shared_ptr<Node>& node, Context::Compound& ctx)
	{
		auto body = node->Children();
		if (!body.size()) {
			return; //TODO: Check if return type is void
		}

		// Process each child node
		for (const auto& childNode : body) {
			auto child = childNode.lock();
			auto returnType = ExecuteStatement(child, ctx);
			switch (returnType)
			{
			case RETURN_RETURN:
				goto done;
			case RETURN_BREAK:
				CryImplExcept(); //TODO: break statement not within loop or switch
			}
		}
	done: {}
	}

	enum { RETURN_NORMAL, RETURN_BREAK, RETURN_RETURN };

	int ExecuteStatement(const std::shared_ptr<AST::ASTNode>& childNode, Context::Compound& ctx)
	{
		using namespace AST;

		switch (childNode->Label())
		{
		case NodeID::COMPOUND_STMT_ID: {
			auto node = Util::NodeCast<CompoundStmt>(childNode);
			CreateCompound(node, ctx);
			break;
		}
		case NodeID::CALL_EXPR_ID: {
			auto node = Util::NodeCast<CallExpr>(childNode);
			CallExpression(node, ctx);
			break;
		}
		case NodeID::DECL_STMT_ID: {
			auto node = Util::NodeCast<DeclStmt>(childNode);
			ProcessDeclaration(node, ctx);
			break;
		}
		case NodeID::IF_STMT_ID: {
			auto node = Util::NodeCast<IfStmt>(childNode);
			ProcessCondition(node, ctx);
			break;
		}
		case NodeID::SWITCH_STMT_ID: {
			auto node = Util::NodeCast<SwitchStmt>(childNode);
			if (ProcessSwitch(node, ctx) == RETURN_RETURN) {
				return RETURN_RETURN;
			}
			break;
		}
		case NodeID::WHILE_STMT_ID: {
			auto node = Util::NodeCast<WhileStmt>(childNode);
			ProcessWhileLoop(node, ctx);
			break;
		}
		case NodeID::DO_STMT_ID: {
			auto node = Util::NodeCast<DoStmt>(childNode);
			ProcessDoLoop(node, ctx);
			break;
		}
		case NodeID::FOR_STMT_ID: {
			auto node = Util::NodeCast<ForStmt>(childNode);
			ProcessForLoop(node, ctx);
			break;
		}
		case NodeID::BREAK_STMT_ID: {
			return RETURN_BREAK;
		}
		case NodeID::RETURN_STMT_ID: {
			auto node = Util::NodeCast<ReturnStmt>(childNode);
			Context::Function funcCtx = ctx->FindContext<FunctionContext>(Context::tag::FUNCTION);
			assert(funcCtx);
			ProcessReturn(node, funcCtx);
			return RETURN_RETURN;
		}
		default:
			ProcessExpression(childNode, ctx);
		}

		return RETURN_NORMAL;
	}

	// If all else fails, try the node as expression
	void ProcessExpression(const std::shared_ptr<AST::ASTNode>& node, Context::Compound& ctx)
	{
		ResolveExpression(node, ctx);
	}

	// Register the declaration in the current context scope
	void ProcessDeclaration(std::shared_ptr<DeclStmt>& declNode, Context::Compound& ctx)
	{
		for (const auto& child : declNode->Children()) {
			auto node = Util::NodeCast<VarDecl>(child);

			//TODO: Super ugly & wrong. Create the value fromt he valDecl return type,
			//      then optionally initialize the value with an expression.
			auto value = std::make_shared<Valuedef::Value>(Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::INT));
			if (node->HasExpression()) {
				auto initializerValue = ResolveExpression(node->Expression(), ctx);
				value->ReplaceValueWithValue(*initializerValue.get());
			}

			ctx->PushVar(node->Identifier(), value);
		}
	}

	void ProcessCondition(std::shared_ptr<IfStmt>& node, Context::Compound& ctx)
	{
		auto value = ResolveExpression(node->Expression(), ctx);
		if (Util::EvaluateAsBoolean(value)) {
			if (node->HasTruthCompound()) {
				auto continueNode = node->TruthCompound();
				auto compoundNode = Util::NodeCast<CompoundStmt>(continueNode);
				ProcessCompound(compoundNode, ctx);
			}
		}
		// Handle alternative path, if any
		else if (node->HasAltCompound()) {
			auto continueNode = node->AltCompound();
			auto compoundNode = Util::NodeCast<CompoundStmt>(continueNode);
			ProcessCompound(compoundNode, ctx);
		}
	}

	int ProcessSwitch(std::shared_ptr<SwitchStmt>& node, Context::Compound& ctx)
	{
		if (!node->HasBodyExpression()) { return RETURN_NORMAL; } //TODO: set warning about useless statement

		// Body node must be compound in order to be executable
		if (node->BodyExpression()->Label() != AST::NodeID::COMPOUND_STMT_ID) {
			//TODO: set warning about non-executable
			return RETURN_NORMAL;
		}

		auto value = ResolveExpression(node->Expression(), ctx);
		auto compoundNode = Util::NodeCast<CompoundStmt>(node->BodyExpression());

		// Process compound within the switch statement instead of calling process compound
		// since the switch body compound semantically differs from a generic compound block.
		auto compCtx = ctx->MakeContext<CompoundContext>();
		{
			auto body = compoundNode->Children();
			if (!body.size()) {
				return RETURN_NORMAL; //TODO: set warning: empty statement
			}

			// Process each child node
			for (const auto& childNode : body) {
				auto child = childNode.lock();
				if (child->Label() != AST::NodeID::CASE_STMT_ID) {
					continue; //TODO: set warning: statement will never be executed
				}
				auto caseNode = Util::NodeCast<CaseStmt>(child);
				if (!Util::IsNodeLiteral(caseNode->Identifier())) {
					CryImplExcept(); //TODO: case label must be integer constant
				}
				auto literal = Util::NodeCast<Literal>(caseNode->Identifier());
				const int caseLabelInt = Util::EvaluateValueAsInteger(literal->Type2());
				const int valueInt = Util::EvaluateValueAsInteger(value);
				if (caseLabelInt == valueInt) {
					return ExecuteStatement(caseNode->Expression(), compCtx);
				}
			}
		}

		return RETURN_NORMAL;
	}

	// Execute body statement as long as expression is true
	void ProcessWhileLoop(std::shared_ptr<WhileStmt>& node, Context::Compound& ctx)
	{
		if (!node->HasBodyExpression()) { return; }
		while (Util::EvaluateAsBoolean(ResolveExpression(node->Expression(), ctx))) {
			ExecuteStatement(node->BodyExpression(), ctx);
		}
	}

	// Execute body statement once and then as long as expression is true
	void ProcessDoLoop(std::shared_ptr<DoStmt>& node, Context::Compound& ctx)
	{
		if (!node->HasBodyExpression()) { return; }
		do {
			ExecuteStatement(node->BodyExpression(), ctx);
		} while (Util::EvaluateAsBoolean(ResolveExpression(node->Expression(), ctx)));
	}

	// Loop over statement unil expression is false
	void ProcessForLoop(std::shared_ptr<ForStmt>& node, Context::Compound& ctx)
	{
		if (!node->HasBodyExpression()) { return; }
		for (ExecuteStatement(node->Declaration(), ctx);
			Util::EvaluateAsBoolean(ResolveExpression(node->Expression(), ctx));
			ExecuteStatement(node->FinishStatement(), ctx)) {
			ExecuteStatement(node->BodyExpression(), ctx);
		}
	}

	// Return from function with either special value or none
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

// Warp startup parameters in program arguments format. To If no arguments are
// passed to the startup the parameters are ignored. The program contract
// defines thee parameters, respectively:
//   1.) argc, the argument count
//   2.) argv, an array to string literal parameters
//   3.) envp, an array to string literal environment variables
void FormatStartupParameters(std::array<std::string, 3> mapper, Parameters&& params, Context::Function& ctx)
{
	if (params.empty()) { return; }

	ctx->PushVar({ mapper[0], Util::MakeInt(static_cast<int>(params.size())) });
	ctx->PushVar({ mapper[1], Util::MakeFloat(872.21) }); //TODO: Util::MakeArray
	ctx->PushVar({ mapper[2], Util::MakeBool(true) }); //TODO: Util::MakeArray
}

} // namespace

// Call program routine from external context
Evaluator& Evaluator::CallRoutine(const std::string& symbol, const ArgumentList& args)
{
	auto funcNode = m_unitContext->LookupSymbol<FunctionDecl>(symbol);
	auto funcCtx = m_unitContext->MakeContext<FunctionContext>(funcNode->Identifier());

	if (funcNode->HasParameters()) {
		std::string argv = "argv"; std::string envp = "envp";
		std::string argc = Util::NodeCast<ParamDecl>(funcNode->ParameterStatement()->Children()[0])->Identifier();
		if (funcNode->ParameterStatement()->ChildrenCount() > 1) {
			argv = Util::NodeCast<ParamDecl>(funcNode->ParameterStatement()->Children()[1])->Identifier();
		}
		if (funcNode->ParameterStatement()->ChildrenCount() > 2) {
			envp = Util::NodeCast<ParamDecl>(funcNode->ParameterStatement()->Children()[2])->Identifier();
		}
		if (funcNode->ParameterStatement()->ChildrenCount() > 3) {
			CryImplExcept(); //TODO
		}
		FormatStartupParameters({ argc, argv, envp }, ConvertToValueDef(std::move(args)), funcCtx);
	}
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
		CryImplExcept();
	}
}

std::string Interpreter::EntryPoint(const char *entry)
{
	if (!entry) {
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
