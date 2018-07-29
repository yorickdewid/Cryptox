// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Interpreter.h"
#include "ExternalMethod.h"

#include <Cry/Except.h>

#include "../../coilcl/src/ValueHelper.h"

#include <numeric>
#include <set>

//TODO:
// - Stacktrace
// - Infinite loop detection
// - Runtime resolve
// - Check .lock()'s

#define RETURN_VALUE 0
#define ENTRY_SYMBOL "main"
#define ADDRESS_SEQUENCE 1000

#define DEFAULT_MAKE_CONTEXT() \
	template<typename ContextType, typename... ArgTypes> \
	std::shared_ptr<ContextType> MakeContext(ArgTypes&&... args) { \
		return std::make_shared<ContextType>(shared_from_this(), std::forward<ArgTypes>(args)...); \
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
		&& Program()->AstPassthrough()->ChildrenCount()
		&& Program()->AstPassthrough()->Parent().expired()
		&& IsTranslationUnitNode(Program()->Ast().Front()));
}

class Evaluator;

namespace EVM
{

class AbstractContext;
class GlobalContext;
class UnitContext;
class CompoundContext;
class FunctionContext;

// Access record as a single entity value. If the record value is not initialized, create a new record value and
// assign it to the value. If the member values does not exist in the record value, create a new member value, add
// it to the record value and return the member value as writable value. If the member value already exist in the
// record value, return the value immediately.
class RecordProxy
{
	// Crate new record field value from record type.
	static std::shared_ptr<Valuedef::Value> MemberFromType(const std::shared_ptr<Valuedef::Value>& recordValue, const std::string& name)
	{
		Typedef::RecordType *recType = ((Typedef::RecordType*)recordValue->Type().operator->());
		const auto fields = recType->Fields();
		auto it = std::find_if(fields.cbegin(), fields.cend(), [=](auto pair) {
			return name == pair.first;
		});
		if (it == fields.cend()) {
			CryImplExcept(); //TODO: 'recType->Name()' has no member named 'member->FieldName()'
		}

		// Create new value from type definition.
		return std::make_shared<Valuedef::Value>(*it->second.get());
	}

	// Create a new record value and assign it to the passed value. This initializes the value as a record. If the 
	// record type was setup with a name, copy the name to the record value.
	static void AssignNewRecord(const std::shared_ptr<Valuedef::Value>& recordValue, Valuedef::RecordValue&& record)
	{
		Typedef::RecordType *recType = ((Typedef::RecordType*)recordValue->Type().operator->());

		// Set record name if known.
		if (!recType->IsAnonymous()) {
			if (!record.HasRecordName()) {
				record.SetRecordName(recType->Name());
			}
		}

		// Assign record value to passed record.
		(*recordValue) = (recType->TypeSpecifier() == Typedef::RecordType::Specifier::STRUCT)
			? Util::MakeStruct(std::move(record))
			: Util::MakeUnion(std::move(record));
	}

public:
	static std::shared_ptr<Valuedef::Value> MemberValue(std::shared_ptr<Valuedef::Value>& recordValue, const std::string& name)
	{
		// If value is not set, create a new record value.
		if (!recordValue->Empty()) {
			Valuedef::RecordValue recVal = recordValue->As<Valuedef::RecordValue>();
			if (recVal.HasField(name)) {
				return recVal.GetField(name);
			}
			else {
				auto memberValue = MemberFromType(recordValue, name);
				recVal.AddField({ name, memberValue });
				AssignNewRecord(recordValue, std::move(recVal));
				return memberValue;
			}
		}
		else {
			Valuedef::RecordValue recVal;
			auto memberValue = MemberFromType(recordValue, name);
			recVal.AddField({ name, memberValue });
			AssignNewRecord(recordValue, std::move(recVal));
			return memberValue;
		}
	}
};

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

using WeakGlobal = std::weak_ptr<GlobalContext>;
using WeakUnit = std::weak_ptr<UnitContext>;
using WeakCompound = std::weak_ptr<CompoundContext>;
using WeakFunction = std::weak_ptr<FunctionContext>;

// Make global context from existing context or create a new global context.
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
	struct OpaqueAddress
	{
		using AddressType = unsigned int;

		AddressType address;

		OpaqueAddress(AddressType address, std::weak_ptr<Valuedef::Value> ptr)
			: address{ address }
			, m_ptr{ ptr }
		{
		}

		inline std::shared_ptr<Valuedef::Value> operator->() const
		{
			return this->Get();
		}

		inline std::shared_ptr<Valuedef::Value> operator*() const
		{
			return this->Get();
		}

		inline std::shared_ptr<Valuedef::Value> Get() const
		{
			return m_ptr.lock();
		}

		inline bool Expired() const noexcept
		{
			return m_ptr.expired();
		}

		bool operator==(const OpaqueAddress& other) const
		{
			return address == other.address;
		}
		bool operator!=(const OpaqueAddress& other) const
		{
			return address != other.address;
		}
		bool operator<(const OpaqueAddress& other) const
		{
			return address < other.address;
		}
		bool operator>(const OpaqueAddress& other) const
		{
			return address > other.address;
		}
		bool operator<=(const OpaqueAddress& other) const
		{
			return address <= other.address;
		}
		bool operator>=(const OpaqueAddress& other) const
		{
			return address >= other.address;
		}

		static AddressType Advance()
		{
			return s_addressSequnce++;
		}

	private:
		std::weak_ptr<Valuedef::Value> m_ptr;

	private:
		static AddressType s_addressSequnce;
	};

	~DeclarationRegistry()
	{
		// Force reset of values in this scope.
		for (auto& map : m_namedMap) {
			std::get<std::shared_ptr<Valuedef::Value>>(map.second).reset();
		}
	}

	//TODO
	//virtual PushVar() = 0;
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

		auto ptr = std::make_shared<Valuedef::Value>(value);
		const auto addr = OpaqueAddress::Advance();
		s_global.emplace(addr, ptr);
		m_namedMap.emplace(key, std::make_tuple(OpaqueAddress{ addr,ptr }, ptr));

		//TODO: remove
		m_localObj.emplace(std::forward<KeyType>(key), std::forward<ValueType>(value));
	}

	//FUTURE: maybe remove?
	void PushVar(std::pair<const std::string, Valuedef::Value>&& pair)
	{
		PushVar(std::move(pair.first), std::move(pair.second));
	}

	// Find the value by identifier, if not found IdentifierNotFoundException is thrown.
	virtual std::weak_ptr<Valuedef::Value> ValueByIdentifier(const std::string& key)
	{
		auto val = m_namedMap.find(key);
		if (val == m_namedMap.end()) {
			throw IdentifierNotFoundException{ key };
		}

		// Get the value part from the tuple.
		return std::get<std::shared_ptr<Valuedef::Value>>(val->second);
	}

	// Find the address by identifier, if not found null should be returned.
	virtual OpaqueAddress AddressByIdentifier(const std::string& key)
	{
		auto val = m_namedMap.find(key);
		if (val == m_namedMap.end()) {
			// TODO: why not null?
			throw IdentifierNotFoundException{ key };
		}

		// Get the address part from the tuple.
		return std::get<OpaqueAddress>(val->second);
	}

	std::shared_ptr<Valuedef::Value> DeclarationReference(const std::shared_ptr<AST::ASTNode>& node)
	{
		std::shared_ptr<DeclRefExpr> declRef;

		switch (node->Label()) {
		case AST::NodeID::DECL_REF_EXPR_ID: {
			declRef = Util::NodeCast<DeclRefExpr>(node);
			return ValueByIdentifier(declRef->Identifier()).lock();
		}
		case AST::NodeID::MEMBER_EXPR_ID: {
			const auto member = Util::NodeCast<MemberExpr>(node);
			std::shared_ptr<Valuedef::Value> value = ValueByIdentifier(member->RecordRef()->Identifier()).lock();
			return RecordProxy::MemberValue(value, member->FieldName());
		}
		}

		CryImplExcept(); //TODO
	}

	// Test if there are any declarations in the current context.
	bool HasLocalValues() const noexcept
	{
		return !m_namedMap.empty();
	}

	// Return number of values in the context.
	size_t LocalValueCount() const noexcept
	{
		return m_namedMap.size();
	}

#ifdef CRY_DEBUG
	void DumpVar(const std::string& key)
	{
		auto var = ValueByIdentifier(key);
		if (!var.lock()) { printf("%s -> (null)", key.c_str()); }
		else {
			printf("%s -> ", key.c_str());
			DUMP_VALUE((*var.lock().get()));
		}
	}
#endif

	// Get value from global address.
	static std::shared_ptr<Valuedef::Value> ValueByAddress(const OpaqueAddress& address)
	{
		return s_global.find(address)->Get();
	}

	// Clear object registery.
	static void ClearGlobalObject() { s_global.clear(); }

protected:
	std::map<std::string, Valuedef::Value> m_localObj; //TODO: remove
	std::map<std::string, std::tuple<OpaqueAddress, std::shared_ptr<Valuedef::Value>>> m_namedMap;

private:
	static std::set<OpaqueAddress> s_global;
};

DeclarationRegistry::OpaqueAddress::AddressType DeclarationRegistry::OpaqueAddress::s_addressSequnce{ ADDRESS_SEQUENCE };
std::set<DeclarationRegistry::OpaqueAddress> DeclarationRegistry::s_global;

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
	// Create and position a value in the special space.
	template<size_t Position>
	void CreateSpecialVar(CoilCl::Valuedef::Value value, bool override = false)
	{
		if (!m_specialType[Position] || override) {
			m_specialType[Position] = std::move(value);
			return;
		}

		throw std::logic_error{ "cannot set special value multiple time" };
	}

	// Test if an return value is set.
	bool HasReturnValue() const noexcept
	{
		return (!!m_specialType[RETURN_VALUE]);
	}

	// Retrieve return value.
	auto ReturnValue() const noexcept
	{
		return m_specialType[RETURN_VALUE].get();
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
	std::array<boost::optional<CoilCl::Valuedef::Value>, 1> m_specialType;
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

	// Run predicate for each context item.
	template<typename Predicate, typename CastAsType = AbstractContext>
	void ForEachContext(Predicate&& pred)
	{
		for (const auto& context : m_contextList) {
			if (auto ptr = context.lock()) {
				pred(std::static_pointer_cast<CastAsType>(context));
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

private:
	virtual std::shared_ptr<AbstractContext> GetSharedSelf()
	{
		return shared_from_this();
	}

private:
	//std::list<std::shared_ptr<AbstractContext>> m_objects;
	std::string m_name;
};

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

	template<typename ContextType, typename... ArgTypes>
	std::shared_ptr<ContextType> MakeContext(ArgTypes&&... args)
	{
		assert(Parent());
		return Detail::MakeContextImpl<ContextType>{ shared_from_this(), Parent() }(std::forward<ArgTypes>(args)...);
	}

	void AttachCompound(Context::Compound& ctx)
	{
		assert(m_bodyContext.expired());
		m_bodyContext = ctx;
	}

	// Find the value by identifier, if not found IdentifierNotFoundException is thrown.
	virtual std::weak_ptr<Valuedef::Value> ValueByIdentifier(const std::string& key) override
	{
		auto val = m_namedMap.find(key);
		if (val == m_namedMap.end()) {

			// If a function compound context was set, then search the context for an identifier. On
			// all program defined functions the context is attached. External modules may define
			// functions that do not set compounds, and thus the body context can by empty.
			if (auto ctx = m_bodyContext.lock()) {
				return CastDownAs<DeclarationRegistry>(ctx)->ValueByIdentifier(key);
			}

			return std::dynamic_pointer_cast<DeclarationRegistry>(Parent())->ValueByIdentifier(key);
		}

		// Get the value part from the tuple.
		return std::get<std::shared_ptr<Valuedef::Value>>(val->second);
	}

private:
	virtual std::shared_ptr<AbstractContext> GetSharedSelf()
	{
		return shared_from_this();
	}

private:
	Context::WeakCompound m_bodyContext;
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
		assert(FindContext<UnitContext>(Context::tag::UNIT));
		return Detail::MakeContextImpl<ContextType>{ shared_from_this(), FindContext<UnitContext>(Context::tag::UNIT) }(std::forward<Args>(args)...);
	}

	// Find the value by identifier, if not found IdentifierNotFoundException is thrown.
	virtual std::weak_ptr<Valuedef::Value> ValueByIdentifier(const std::string& key) override
	{
		auto val = m_namedMap.find(key);
		if (val == m_namedMap.end()) {
			return std::dynamic_pointer_cast<DeclarationRegistry>(Parent())->ValueByIdentifier(key);
		}

		// Get the value part from the tuple.
		return std::get<std::shared_ptr<Valuedef::Value>>(val->second);
	}

private:
	virtual std::shared_ptr<AbstractContext> GetSharedSelf()
	{
		return shared_from_this();
	}
};

} // namespace EVM

class Runnable
{
	void ConvertInternalMethod(std::shared_ptr<FunctionDecl>& funcNode)
	{
		assert(funcNode);
		m_functionData = funcNode;

		// Convert paramters.
		if (!funcNode->HasParameters()) { return; }
		for (const auto& child : funcNode->ParameterStatement()->Children()) {
			if (child.lock()->Label() == AST::NodeID::VARIADIC_DECL_ID) {
				m_paramList.emplace_back("__va_list__");
			}
			else {
				auto paramDecl = Util::NodeCast<ParamDecl>(child);
				assert(paramDecl->HasReturnType());
				m_paramList.emplace_back(paramDecl->Identifier(), paramDecl->ReturnType());
			}
		}
	}

	void ConvertExternalMethod(const ExternalMethod *exfuncRef)
	{
		assert(exfuncRef);
		m_isExternal = true;
		m_functionData = exfuncRef;

		// Copy paramters.
		if (exfuncRef->Parameters().empty()) { return; }
		std::copy(exfuncRef->Parameters().cbegin()
			, exfuncRef->Parameters().cend()
			, std::back_inserter(m_paramList));
	}

public:
	Runnable() = default;

	// Construct runnable from function declaration.
	explicit Runnable(std::shared_ptr<FunctionDecl>& funcNode)
	{
		ConvertInternalMethod(funcNode);
	}

	// Construct runnable from external method.
	explicit Runnable(const ExternalMethod *exfuncRef)
	{
		ConvertExternalMethod(exfuncRef);
	}

	Runnable& operator=(std::shared_ptr<FunctionDecl>& method)
	{
		ConvertInternalMethod(method);
		return (*this);
	}

	Runnable& operator=(const ExternalMethod *method)
	{
		ConvertExternalMethod(method);
		return (*this);
	}

	// Query argument size.
	inline size_t ArgumentSize() const noexcept { return m_paramList.size(); }
	// Query if runnable has arguments.
	inline bool HasArguments() const noexcept { return ArgumentSize() > 0; }
	// Query if runnable is external method.
	inline bool IsExternal() const noexcept { return m_isExternal; }
	// Get the parameter list.
	const ExternalMethod::ParameterList& Parameters() const noexcept { return m_paramList; }

	template<typename CastType>
	auto Data() const { return boost::get<CastType>(m_functionData); }

	operator bool() const { return !m_functionData.empty(); }

private:
	bool m_isExternal = false;
	boost::variant<std::shared_ptr<FunctionDecl>, const ExternalMethod*> m_functionData;
	ExternalMethod::ParameterList m_paramList;
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
	DeclarationRegistry::ClearGlobalObject();
	Unit(static_cast<TranslationUnitDecl&>(m_ast.Front()));
}

// Global scope, evaluate an entire unit and registger
// toplevel objects such as records, variables and functions.
void Evaluator::Unit(const TranslationUnitDecl& node)
{
	using namespace AST;

	assert(m_unitContext);
	for (const auto& weaknode : node.Children()) {
		if (const auto ptr = weaknode.lock()) {
			//TODO: switch can have more elements
			switch (ptr->Label())
			{
			case NodeID::RECORD_DECL_ID: {
				break;
			}
			case NodeID::TYPEDEF_DECL_ID: {
				// Type definitions should already have been resolved in an earlier stage
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
							auto type = Util::NodeCast<Literal>(varDecl->Expression())->Value();
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
				CryImplExcept(); //TODO: THROW: statement or declaration is unqualified in global scope.
			}
		}
	}
}

using Parameters = std::vector<CoilCl::Valuedef::Value>;

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
				// Arithmetic operations.
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
				// Bitwise operations.
				//
			}

		case BinaryOperator::BinOperand::XOR:
			return std::bit_xor<Type>()(left, right);
		case BinaryOperator::BinOperand::OR:
			return std::bit_or<Type>()(left, right);
		case BinaryOperator::BinOperand::AND:
			return std::bit_and<Type>()(left, right);
		case BinaryOperator::BinOperand::SLEFT:
			return BitLeftShift<Type>()(left, right);
		case BinaryOperator::BinOperand::SRIGHT:
			return BitRightShift<Type>()(left, right);

			{
				//
				// Comparisons.
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
				// Logical operations.
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

namespace ExternalRoutine
{

void Invoke(const ExternalMethod *method, Context::Function& ctx)
{
	assert(method);

	Context::Compound compCtx = ctx->MakeContext<CompoundContext>();
	ctx->AttachCompound(compCtx);
	ExternalFunctionContext exCtx{ [&compCtx](const std::string& name) -> std::shared_ptr<CoilCl::Valuedef::Value>
	{
		return compCtx->ValueByIdentifier(name).lock();
	} };
	method->Call(exCtx);
	auto ptr = exCtx.GetReturn();
	if (ptr) {
		ctx->CreateSpecialVar<RETURN_VALUE>(*ptr.get());
	}

	ptr.reset();
	compCtx.reset();
}

} // namespace ExternalRoutine

namespace InternalRoutine
{

enum { RETURN_NORMAL, RETURN_BREAK, RETURN_RETURN };

int ExecuteStatement(const std::shared_ptr<AST::ASTNode>&, Context::Compound&);

template<typename OperandPred, typename ContainerType = CoilCl::Valuedef::Value>
static CoilCl::Valuedef::Value BinaryOperation(OperandPred predicate, ContainerType&& valuesLHS, ContainerType&& valuesRHS)
{
	typename OperandPred::result_type result = predicate(
		valuesLHS.As<OperandPred::result_type>(),
		valuesRHS.As<OperandPred::result_type>());

	return Util::MakeInt(result); //TODO: not always an integer
}

// Inverse the boolea result.
CoilCl::Valuedef::Value EvaluateInverse(const CoilCl::Valuedef::Value& value)
{
	return Util::MakeBool(!Util::EvaluateValueAsBoolean(value));
}

//FUTURE: both operations can be improved.
template<int Increment, typename OperandPred, typename ContextType>
CoilCl::Valuedef::Value ValueAlteration(OperandPred predicate, AST::UnaryOperator::OperandSide side, std::shared_ptr<AST::ASTNode> node, ContextType& ctx)
{
	std::shared_ptr<CoilCl::Valuedef::Value> value = ctx->DeclarationReference(node);
	int result = predicate(value->As<int>(), Increment); //TODO: not always an integer

	// On postfix operand, copy the original first.
	if (side == AST::UnaryOperator::OperandSide::POSTFIX) {
		auto origvalue = CoilCl::Valuedef::Value{ (*value.get()) };
		(*value) = Util::MakeInt(result); //TODO: not always an integer
		return origvalue;
	}

	// On prefix, perform the unary operand on the original.
	(*value) = Util::MakeInt(result); //TODO: not always an integer
	return (*value.get());
}

template<typename ContextType>
CoilCl::Valuedef::Value ValueReference(std::shared_ptr<AST::ASTNode> node, ContextType& ctx)
{
	const auto declRef = Util::NodeCast<DeclRefExpr>(node);
	DeclarationRegistry::OpaqueAddress address = ctx->AddressByIdentifier(declRef->Identifier());
	assert(!address.Expired());

	//TODO: Util::MakePointer(address.address);
	return Util::MakeInt(address.address);
}

template<typename ContextType>
CoilCl::Valuedef::Value ResolveExpression(std::shared_ptr<AST::ASTNode> node, ContextType& ctx)
{
	switch (node->Label())
	{
		{
			//
			// Return literal types.
			//
		}

	case AST::NodeID::CHARACTER_LITERAL_ID: {
		return Util::NodeCast<CharacterLiteral>(node)->Value();
	}
	case AST::NodeID::STRING_LITERAL_ID: {
		return Util::NodeCast<StringLiteral>(node)->Value();
	}
	case AST::NodeID::INTEGER_LITERAL_ID: {
		return Util::NodeCast<IntegerLiteral>(node)->Value();
	}
	case AST::NodeID::FLOAT_LITERAL_ID: {
		return Util::NodeCast<FloatingLiteral>(node)->Value();
	}
	case AST::NodeID::INIT_LIST_EXPR_ID: {
		std::vector<int> dummyArray; //TODO: only only integer
		const auto list = Util::NodeCast<InitListExpr>(node)->List();
		std::transform(list.cbegin(), list.cend(), std::back_inserter(dummyArray), [&ctx](const std::shared_ptr<AST::ASTNode>& value) -> int
		{
			return ResolveExpression(value, ctx).As<int>();
		});

		return Util::MakeIntArray(dummyArray);
	}

	{
		//
		// Operators.
		//
	}

	case AST::NodeID::BINARY_OPERATOR_ID: {
		const auto op = std::dynamic_pointer_cast<BinaryOperator>(node);

		// If the binary operand is an assignment do it right now.
		if (op->Operand() == BinaryOperator::BinOperand::ASSGN) {

			// The left hand side must be a lvalue and thus can be converted into an declaration
			// reference. The declaration reference value is altered when the new value is assigned
			// and as a consequence updates the declaration table entry.
			const auto assignValue = ctx->DeclarationReference(op->LHS());

			//const auto assignValue = ctx->ValueByIdentifier(declRef->Identifier()).lock();
			(*assignValue) = ResolveExpression(op->RHS(), ctx);
			return (*assignValue.get());
		}

		auto lhsValue = ResolveExpression(op->LHS(), ctx);
		auto rhsValue = ResolveExpression(op->RHS(), ctx);
		return BinaryOperation(OperandFactory<int>(op->Operand()), lhsValue, rhsValue); //TODO: not always an integer
	}
	case AST::NodeID::CONDITIONAL_OPERATOR_ID: {
		const auto op = std::dynamic_pointer_cast<ConditionalOperator>(node);
		auto value = ResolveExpression(op->Expression(), ctx);
		if (Util::EvaluateValueAsBoolean(value)) {
			return ResolveExpression(op->TruthStatement(), ctx);
		}

		// Handle alternative path.
		return ResolveExpression(op->AltStatement(), ctx);
	}
	case AST::NodeID::UNARY_OPERATOR_ID: {
		const auto op = std::dynamic_pointer_cast<AST::UnaryOperator>(node);
		switch (op->Operand())
		{
		case AST::UnaryOperator::UnaryOperand::INC:
			return ValueAlteration<1>(std::plus<int>(), op->OperationSide(), op->Expression(), ctx); //TODO: not always an integer
		case AST::UnaryOperator::UnaryOperand::DEC:
			return ValueAlteration<1>(std::minus<int>(), op->OperationSide(), op->Expression(), ctx); //TODO: not always an integer

			/*
			case AST::UnaryOperator::UnaryOperand::INTPOS:
				return ValueSignedness();
			case AST::UnaryOperator::UnaryOperand::INTNEG:
				return ValueSignedness();
			*/

		case AST::UnaryOperator::UnaryOperand::ADDR:
			return ValueReference(op->Expression(), ctx);
		case AST::UnaryOperator::UnaryOperand::PTRVAL:
			//ValueIndirection();
			break;

			//case AST::UnaryOperator::UnaryOperand::BITNOT:
		case AST::UnaryOperator::UnaryOperand::BOOLNOT:
			return EvaluateInverse(ResolveExpression(op->Expression(), ctx));
		}
		CryImplExcept(); //TODO:
	}
	case AST::NodeID::COMPOUND_ASSIGN_OPERATOR_ID: {
		Util::NodeCast<CompoundAssignOperator>(node);
		break;
	}

	{
		//
		// Return routine result.
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
		// Lookup symbol reference.
		//
	}

	case AST::NodeID::MEMBER_EXPR_ID:
	case AST::NodeID::DECL_REF_EXPR_ID: {
		const auto value = ctx->DeclarationReference(node);
		return (*value.get());
	}

	{
		//
		// Type casting.
		//
	}

	case AST::NodeID::IMPLICIT_CONVERTION_EXPR_ID: {
		auto convRef = Util::NodeCast<ImplicitConvertionExpr>(node);
		CRY_UNUSED(convRef);
		return Util::MakeInt(12); //TODO: for now
	}

	{
		//
		// Enclosed expression.
		//
	}

	case AST::NodeID::PAREN_EXPR_ID: {
		auto expr = Util::NodeCast<ParenExpr>(node);
		assert(expr->HasExpression());
		return ResolveExpression(expr->Expression(), ctx);
	}

	default:
		break;
	}

	CryImplExcept(); //TODO
}

template<typename ContextType>
Context::Function CallExpression(const std::shared_ptr<CallExpr>& callNode, ContextType& ctx)
{
	assert(callNode);
	assert(callNode->ChildrenCount());
	assert(callNode->FunctionReference()->IsResolved());
	const std::string& functionIdentifier = callNode->FunctionReference()->Identifier();
	Runnable function;

	// Create a new function context. Depending on the parent context the new context
	// is a direct hierarchical or a sub-hierarchical child.
	Context::Function funcCtx = ctx->MakeContext<FunctionContext>(functionIdentifier);
	assert(!funcCtx->HasLocalValues());

	// The symbol is can be found in different places. The interpreter will locate
	// the runnable object according to the following algorithm:
	//   1.) Look for the symbol in the current program assuming it is a local object
	//   2.) Request the symbol as an external routine (internal or external module)
	//   3.) Throw an symbol not found exception halting from further execution
	if (auto funcNode = ctx->FindContext<UnitContext>(Context::tag::UNIT)->LookupSymbol<FunctionDecl>(functionIdentifier)) {
		function = funcNode;
	}
	else if (auto exfuncRef = GlobalExecutionState::FindExternalSymbol(functionIdentifier)) {
		function = exfuncRef;
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
		else if (function.ArgumentSize() < argsDecls.size() && !function.Parameters().back().IsVariadic()) {
			CryImplExcept(); //TODO: source.c:0:0: error: too few arguments to function 'funcNode'
		}

		// Assign function arguments to parameters
		int i = 0;
		auto itArgs = argsDecls.cbegin();
		while (itArgs != argsDecls.cend()) {
			if (function.Parameters().at(i).Empty()) {
				CryImplExcept(); //TODO: source.c:0:0: error: parameter name omitted to function 'funcNode'
			}
			if (function.Parameters().at(i).IsVariadic()) {
				int v_i = 0;
				while (itArgs != argsDecls.cend()) {
					auto value = ResolveExpression(itArgs->lock(), ctx);
					std::string autoVA{ "__va_arg" + std::to_string(v_i++) + "__" };
					funcCtx->PushVar(autoVA, Valuedef::Value{ value });
					++itArgs;
				}
				break;
			}
			else {
				auto value = ResolveExpression(itArgs->lock(), ctx);
				if (function.Parameters().at(i).DataType() != value.Type()) {
					CryImplExcept(); //TODO: source.c:0:0: error: cannot convert argument of type 'X' to parameter type 'Y'
				}
				//TODO: check if param is pointer
				funcCtx->PushVar(function.Parameters().at(i).Identifier(), Valuedef::Value{ value });
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
		ExternalRoutine::Invoke(function.Data<const ExternalMethod*>(), funcCtx);
	}
	else {
		auto func = function.Data<std::shared_ptr<FunctionDecl>>();
		InternalRoutine::Invoke(func, funcCtx);
	}

	return funcCtx;
}

// Run all nodes in the compound.
template<typename Node>
void ProcessCompound(const std::shared_ptr<Node>& node, Context::Compound& ctx)
{
	auto body = node->Children();
	if (!body.size()) {
		return; //TODO: Check if return type is void
	}

	// Process each child node.
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

// Call internal function.
void Invoke(std::shared_ptr<FunctionDecl>& funcNode, Context::Function& ctx)
{
	assert(funcNode->ChildrenCount());
	if (funcNode->HasParameters()) {
		//TODO: Do something?
	}

	Context::Compound compCtx = ctx->MakeContext<CompoundContext>();
	ctx->AttachCompound(compCtx);
	ProcessCompound(const_cast<std::shared_ptr<CompoundStmt>&>(funcNode->FunctionCompound()), compCtx);
	compCtx.reset();
}

// If all else fails, try the node as expression.
void ProcessExpression(const std::shared_ptr<AST::ASTNode>& node, Context::Compound& ctx)
{
	// Only execute the expression and ignore the result.
	ResolveExpression(node, ctx);
}

// Register the declaration in the current context scope.
void ProcessDeclaration(std::shared_ptr<DeclStmt>& declNode, Context::Compound& ctx)
{
	for (const auto& child : declNode->Children()) {
		auto node = Util::NodeCast<VarDecl>(child);
		assert(node->HasReturnType());

		auto value = Valuedef::Value{ node->ReturnType() };
		if (node->HasExpression()) {
			value = ResolveExpression(node->Expression(), ctx);
		}

		ctx->PushVar(node->Identifier(), value);
	}
}

// Run the expression and evaluate return values as boolean.
int ProcessCondition(std::shared_ptr<IfStmt>& node, Context::Compound& ctx)
{
	auto value = ResolveExpression(node->Expression(), ctx);
	if (Util::EvaluateValueAsBoolean(value)) {
		if (node->HasTruthCompound()) {
			auto continueNode = node->TruthCompound();
			if (Util::IsNodeCompound(continueNode)) {
				auto compoundNode = Util::NodeCast<CompoundStmt>(continueNode);
				ProcessCompound(compoundNode, ctx);
			}
			else {
				auto compoundNode = Util::NodeCast<AST::ASTNode>(continueNode);
				return ExecuteStatement(compoundNode, ctx);
			}
		}
	}
	// Handle alternative path, if defined.
	else if (node->HasAltCompound()) {
		auto continueNode = node->AltCompound();
		if (Util::IsNodeCompound(continueNode)) {
			auto compoundNode = Util::NodeCast<CompoundStmt>(continueNode);
			ProcessCompound(compoundNode, ctx);
		}
		else {
			auto compoundNode = Util::NodeCast<AST::ASTNode>(continueNode);
			return ExecuteStatement(compoundNode, ctx);
		}
	}

	return RETURN_NORMAL;
}

// Process the switch statement.
int ProcessSwitch(std::shared_ptr<SwitchStmt>& node, Context::Compound& ctx)
{
	if (!node->HasBodyExpression()) { return RETURN_NORMAL; } //TODO: set warning about useless statement

	// Body node must be compound in order to be executable.
	if (node->BodyExpression()->Label() != AST::NodeID::COMPOUND_STMT_ID) {
		//TODO: set warning about non-executable
		return RETURN_NORMAL;
	}

	auto value = ResolveExpression(node->Expression(), ctx);
	auto compoundNode = Util::NodeCast<CompoundStmt>(node->BodyExpression());

	// Process compound within the switch statement instead of calling process compound
	// since the switch body compound semantically differs from a generic compound block.
	Context::Compound compCtx = ctx->MakeContext<CompoundContext>();
	{
		auto body = compoundNode->Children();
		if (!body.size()) {
			return RETURN_NORMAL; //TODO: set warning: empty statement
		}

		// Process each child node.
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
			const int caseLabelInt = Util::EvaluateValueAsInteger(literal->Value());
			const int valueInt = Util::EvaluateValueAsInteger(value);
			if (caseLabelInt == valueInt) {
				return ExecuteStatement(caseNode->Expression(), compCtx);
			}
		}
	}

	compCtx.reset();
	return RETURN_NORMAL;
}

// Execute body statement as long as expression is true.
void ProcessWhileLoop(std::shared_ptr<WhileStmt>& node, Context::Compound& ctx)
{
	if (!node->HasBodyExpression()) { return; }
	while (Util::EvaluateValueAsBoolean(ResolveExpression(node->Expression(), ctx))) {
		ExecuteStatement(node->BodyExpression(), ctx);
	}
}

// Execute body statement once and then as long as expression is true.
void ProcessDoLoop(std::shared_ptr<DoStmt>& node, Context::Compound& ctx)
{
	if (!node->HasBodyExpression()) { return; }
	do {
		ExecuteStatement(node->BodyExpression(), ctx);
	} while (Util::EvaluateValueAsBoolean(ResolveExpression(node->Expression(), ctx)));
}

// Loop over statement unil expression is false.
void ProcessForLoop(std::shared_ptr<ForStmt>& node, Context::Compound& ctx)
{
	if (!node->HasBodyExpression()) { return; }
	for (ExecuteStatement(node->Declaration(), ctx);
		Util::EvaluateValueAsBoolean(ResolveExpression(node->Expression(), ctx));
		ExecuteStatement(node->FinishStatement(), ctx)) {
		ExecuteStatement(node->BodyExpression(), ctx);
	}
}

// Return from function with either special value or none.
void ProcessReturn(std::shared_ptr<ReturnStmt>& node, Context::Function& ctx)
{
	// Create explicit return type.
	if (!node->HasExpression()) {
		//TODO: Why not empty?
		//ctx->CreateSpecialVar<RETURN_VALUE>(CoilCl::Util::MakeVoid());
		return;
	}

	// Resolve return expression.
	ctx->CreateSpecialVar<RETURN_VALUE>(ResolveExpression(node->Expression(), ctx));
}

// Create new compound context.
void CreateCompound(const std::shared_ptr<AST::ASTNode>& node, Context::Compound& ctx)
{
	auto compNode = Util::NodeCast<CompoundStmt>(node);
	Context::Compound compCtx = ctx->MakeContext<CompoundContext>();
	ProcessCompound(compNode, compCtx);
	compCtx.reset();
}

// Run the statement.
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
		if (ProcessCondition(node, ctx) == RETURN_RETURN) {
			return RETURN_RETURN;
		}
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

} // namespace InternalRoutine

namespace
{

// Convert user defined argument list items to parameters.
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
//   1.) argc, the argument count.
//   2.) argv, an array to string literal parameters.
//   3.) envp, an array to string literal environment variables.
void FormatStartupParameters(std::array<std::string, 3> mapper, Parameters&& params, Context::Function& ctx)
{
	if (params.empty()) { return; }

	//TODO: Capture the environment variables

	ctx->PushVar({ mapper[0], Util::MakeInt(static_cast<int>(params.size())) });
	ctx->PushVar({ mapper[1], Util::MakeFloatArray({872.21f}) }); //TODO: Util::MakeArray
	ctx->PushVar({ mapper[2], Util::MakeString("test") }); //TODO: Util::MakeArray
}

} // namespace

// Call program routine from external context.
Evaluator& Evaluator::CallRoutine(const std::string& symbol, const ArgumentList& args)
{
	auto funcNode = m_unitContext->LookupSymbol<FunctionDecl>(symbol);
	Context::Function funcCtx = m_unitContext->MakeContext<FunctionContext>(funcNode->Identifier());

	// If the entry function does not accept parameters, then there is no point in
	// converting the passed arguments. Likewise skip argument parsing if there are
	// no commandline arguments supplied.
	if (funcNode->HasParameters() && !args.empty()) {
		std::string argv = "argv";
		std::string envp = "envp";
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

	// Go run the startup routine.
	InternalRoutine::Invoke(funcNode, funcCtx);

	// If the function context contained a return value, set the return value as program exit
	// code. When the program is finished, this global return value serves as the exit code.
	// If any other functions are called within the global scope, then the last set return value
	// determines the exit code.
	if (funcCtx->HasReturnValue()) {
		funcCtx->FindContext<GlobalContext>(Context::tag::GLOBAL)->CreateSpecialVar<RETURN_VALUE>(funcCtx->ReturnValue(), true);
	}
	funcCtx.reset();
	return (*this);
}

int Evaluator::YieldResult()
{
	try {
		auto globalCtx = m_unitContext->ParentAs<GlobalContext>();
		if (globalCtx->HasReturnValue()) {
			return globalCtx->ReturnValue().As<int>();
		}
	}
	// On casting faillure, return faillure all the way.
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

// Run the program with current strategy.
Interpreter::ReturnCode Interpreter::Execute(const std::string& entry, const ArgumentList& args, const ArgumentList& envs)
{
	CRY_UNUSED(envs);

	// Check if entry exists in this program.
	PreliminaryCheck(entry);

	return Evaluator::CallFunction(Program()->Ast(), entry, args);
}
