// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Valuedef.h"
#include "TypeFacade.h"
#include "Converter.h"
#include "RefCount.h"
#include "NodeId.h"
#include "SourceLocation.h"
#include "ASTState.h"
#include "ASTTrait.h"
#include "UserData.h"

#include <Cry/Cry.h>
#include <Cry/Serialize.h>

#include <boost/any.hpp>

#include <vector>
#include <memory>
#include <sstream>

#define PRINT_NODE(n) \
	virtual const std::string NodeName() const { \
		return std::string{ RemoveClassFromName(typeid(n).name()) } + " {" + std::to_string(m_state.Alteration()) + "}" + " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + ">"; \
	}

#define NODE_UPCAST(c) \
	std::dynamic_pointer_cast<ASTNode>(c)

#define POLY_IMPL() \
	std::shared_ptr<ASTNode> PolySelf() { \
		return this->GetSharedSelf(); \
	}

#define BUMP_STATE() \
	m_state.Bump((*this));

#define NODE_ID(i) \
	static const AST::NodeID nodeId = i;

#define LABEL() \
	virtual AST::NodeID Label() const noexcept override { return nodeId; }

#define SERIALIZE(p) \
	virtual void Serialize(Serializable::Interface& pack) { \
		pack << nodeId; p::Serialize(pack); \
	}

#define DESERIALIZE(p) \
	virtual void Deserialize(Serializable::Interface& pack) { \
		AST::NodeID _nodeId; pack >> _nodeId; AssertNode(_nodeId, nodeId); p::Deserialize(pack); \
	}

using namespace CoilCl;

template<typename _Ty>
constexpr std::string RemoveClassFromName(_Ty *_name)
{
	constexpr const char stripClassStr[] = "class";
	std::string f{ _name };
	if (size_t pos = f.find_last_of(stripClassStr) != std::string::npos) {
		return f.substr(pos + sizeof(stripClassStr) - 1);
	}
	return f;
}

namespace CoilCl
{
namespace AST
{

class ASTNode;

} // namespace CoilCl
} // namespace AST

template <typename _Ty>
struct Identity { using type = typename _Ty::value_type; };

template<typename Node, typename BaseNode = CoilCl::AST::ASTNode>
class SelfReference : public std::enable_shared_from_this<Node>
{
protected:
	std::shared_ptr<BaseNode> GetSharedSelf()
	{
		return this->shared_from_this();
	}
};

class UniqueObj
{
public:
	using unique_type = int;

private:
	mutable unique_type id;

public:
	inline UniqueObj()
	{
		id = ++_id;
	}

	inline auto& Id() const noexcept { return id; }

	bool operator==(const UniqueObj& other) const noexcept
	{
		return id == other.id;
	}

	bool operator!=(const UniqueObj& other) const noexcept
	{
		return id != other.id;
	}

private:
	static int _id;
};

class Returnable
{
	AST::TypeFacade m_returnType;

protected:
	// Take no return type by default. The caller can query this
	// interface to ask if an return type was set.
	Returnable() = default;

	// Initialize object with return type
	Returnable(AST::TypeFacade type)
		: m_returnType{ type }
	{
	}

public:
	virtual bool HasReturnType() const { return m_returnType.HasValue(); }
	virtual void SetReturnType(AST::TypeFacade type) { m_returnType = type; }
	virtual const AST::TypeFacade& ReturnType() const { return m_returnType; }
	virtual AST::TypeFacade& UpdateReturnType() { return m_returnType; }
};

struct Serializable
{
	struct ChildGroupInterface
	{
		virtual void SaveNode(std::shared_ptr<CoilCl::AST::ASTNode>&) = 0;
		virtual void SaveNode(nullptr_t) = 0;
		virtual int LoadNode(int) = 0;

		// Set size interface
		virtual void SetSize(size_t sz) = 0;
		// Get size interface
		virtual size_t GetSize() noexcept = 0;
	};

	class ChildGroupFacade;

	using GroupListType = std::vector<std::shared_ptr<ChildGroupInterface>>;

	class Interface
	{
		GroupListType m_childGroups;

		virtual GroupListType CreateChildGroups(size_t size) = 0;
		virtual GroupListType GetChildGroups() = 0;

	public:
		// Set the node id
		virtual void SetId(int id) = 0;
		// Invoke registered callbacks
		virtual void FireDependencies(std::shared_ptr<CoilCl::AST::ASTNode>&) = 0;

		// Stream out operators
		virtual void operator<<(int) = 0;
		virtual void operator<<(double) = 0;
		virtual void operator<<(bool) = 0;
		virtual void operator<<(AST::NodeID) = 0;
		virtual void operator<<(std::string) = 0;
		virtual void operator<<(std::vector<uint8_t>) = 0;

		// Stream in operators
		virtual void operator>>(int&) = 0;
		virtual void operator>>(double&) = 0;
		virtual void operator>>(bool&) = 0;
		virtual void operator>>(AST::NodeID&) = 0;
		virtual void operator>>(std::string&) = 0;
		virtual void operator>>(std::vector<uint8_t>&) = 0;

		// Callback operations
		virtual void operator<<=(std::pair<int, std::function<void(const std::shared_ptr<CoilCl::AST::ASTNode>&)>>) = 0;

		ChildGroupFacade ChildGroups(size_t size = 0)
		{
			if (size > 0) {
				// Create child groups
				m_childGroups = CreateChildGroups(size);
			}
			else {
				// Retrieve child groups
				m_childGroups = GetChildGroups();
			}
			return m_childGroups.begin();
		}
	};

	//FUTURE: More methods and operators to communicate data back and forth
	class ChildGroupFacade final
	{
		GroupListType::iterator m_it;
		GroupListType::iterator m_beginIt;

	public:
		ChildGroupFacade(GroupListType::iterator it)
			: m_it{ it }
			, m_beginIt{ it }
		{
			assert(m_it == m_beginIt);
		}

		// Get group id
		auto Id() const { return std::distance(m_beginIt, m_it) + 1; }

		template<typename _Ty, typename = typename std::enable_if<std::is_base_of<CoilCl::AST::ASTNode, _Ty>::value>::type>
		void operator<<(std::shared_ptr<_Ty> ptr)
		{
			if (!ptr) {
				(*m_it)->SaveNode(nullptr);
				return;
			}

			auto astNode = std::dynamic_pointer_cast<CoilCl::AST::ASTNode>(ptr);
			(*m_it)->SaveNode(astNode);
		}
		template<typename _Ty, typename = typename std::enable_if<std::is_base_of<CoilCl::AST::ASTNode, _Ty>::value>::type>
		void operator<<(std::weak_ptr<_Ty> ptr)
		{
			if (std::shared_ptr<CoilCl::AST::ASTNode> astNode = ptr.lock()) {
				(*m_it)->SaveNode(astNode);
				return;
			}

			(*m_it)->SaveNode(nullptr);
		}

		// Return node id
		int operator[](size_t idx)
		{
			return (*m_it)->LoadNode(static_cast<int>(idx));
		}

		// Move iterator forward
		void operator++() { ++m_it; }
		void operator++(int) { m_it++; }

		// Move iterator backward
		void operator--() { --m_it; }
		void operator--(int) { m_it--; }

		// Move iterator
		void Next() { ++m_it; }
		void Previous() { --m_it; }

		// Get or set element size in group
		size_t Size(size_t sz = 0)
		{
			if (sz > 0) {
				(*m_it)->SetSize(sz);
				return sz;
			}

			return (*m_it)->GetSize();
		}

		std::vector<std::shared_ptr<CoilCl::AST::ASTNode>> Children() { return {}; }
	};

	// Serialize interface
	virtual void Serialize(Interface&) = 0;
	// Deserialize interface
	virtual void Deserialize(Interface&) = 0;

protected:
	void AssertNode(const AST::NodeID& got, const AST::NodeID& exp)
	{
		if (got != exp) {
			throw 2; //TODO: throw something usefull
		}
	}
};

struct ModifierInterface
{
	// Emplace object, and push current object one stage down
	virtual void Emplace(size_t, const std::shared_ptr<CoilCl::AST::ASTNode>&&) = 0;
	// Get modifier count
	virtual size_t ModifierCount() const = 0;
};

struct VisitorInterface
{
	struct AbstractVisitor
	{
		//void Visit();
	};

	virtual void Apply(AbstractVisitor&) {}
};

//
// Forward declarations
//

class DeclRefExpr;
class CompoundStmt;
class ArgumentStmt;
class ParamStmt;

namespace CoilCl
{
namespace AST
{

class ASTNode
	: public UniqueObj
	, public VisitorInterface
	, public ModifierInterface
	, virtual public Serializable
{
	NODE_ID(AST::NodeID::AST_NODE_ID);

protected:
	using _MyTy = ASTNode;

protected:
	//TODO: Replace with Util::SourceLocation
	mutable int line = -1;
	mutable int col = -1;

public:
	ASTNode() = default;
	ASTNode(int _line, int _col);

	//
	// Node modifiers
	//

	inline size_t ChildrenCount() const noexcept { return children.size(); }
	inline size_t ModifierCount() const { return m_state.Alteration(); }

	virtual void Emplace(size_t idx, const std::shared_ptr<ASTNode>&& node)
	{
		CRY_UNUSED(idx);
		CRY_UNUSED(node);

		assert(0);
	}

	//
	// Source location operations
	//

	// Set source location
	void SetLocation(int, int) const;
	// Set source location as pair
	void SetLocation(const std::pair<int, int>&) const;
	// Set source location as object
	void SetLocation(Util::SourceLocation&&);
	// Get source location
	std::pair<int, int> Location() const;

	//
	// Abstract function interfaces
	//

	// Get node name
	virtual const std::string NodeName() const = 0;
	// Copy self and cast as base node
	virtual std::shared_ptr<ASTNode> PolySelf() = 0;

	enum struct Traverse
	{
		STAGE_FIRST = 0,
		STAGE_LAST = -1
	};

	template<Traverse Version>
	inline void Print() { this->Print(static_cast<int>(Version)); }

	//TODO: replace with algorithm ?
	void Print(int version, int level = 0, bool last = 0, std::vector<int> ignore = {}) const;

	//
	// Node relations
	//

	//TODO: friend
	// Forward the random access operator on the child node list
	std::weak_ptr<ASTNode> operator[](int idx)
	{
		return children[idx];
	}

	//TODO: friend
	// Forward the random access operator on the child node list
	std::weak_ptr<ASTNode> At(int idx)
	{
		return children[idx];
	}

	//TODO: friend
	std::weak_ptr<ASTNode> Parent()
	{
		return m_parent;
	}

	//TODO: friend
	const std::vector<std::weak_ptr<ASTNode>>& Children()
	{
		return children;
	}

	template<typename _Ty, typename = typename std::enable_if<std::is_pointer<_Ty>::value>::type>
	void AddUserData(_Ty data)
	{
		m_userData.emplace_back(data);
	}

	/*template<typename _Pred>
	auto UserData(_Pred predicate)
	{
		return std::find_if(m_userData.begin(), m_userData.end(), predicate);
	}*/

	virtual NodeID Label() const noexcept { return nodeId; }

	//TODO: friend
	void UpdateDelegate()
	{
		for (auto& wPtr : children) {
			if (auto ptr = wPtr.lock()) {
				ptr->SetParent(std::move(this->PolySelf()));
			}
		}
	}

	//
	// Node serialization interfaces
	//

	// Serialize base node
	virtual void Serialize(Serializable::Interface& pack);
	// Deserialzie base node
	virtual void Deserialize(Serializable::Interface& pack);

protected:
	virtual void AppendChild(const std::shared_ptr<ASTNode>& node)
	{
		children.push_back(node);
	}

	virtual void RemoveChild(size_t idx)
	{
		assert(idx < children.size());
		children.erase(children.begin() + idx);
	}

	void SetParent(const std::shared_ptr<ASTNode>&& node)
	{
		m_parent = node;
	}

protected:
	CoilCl::AST::ASTState<_MyTy> m_state;
	std::vector<std::weak_ptr<_MyTy>> children;
	std::weak_ptr<_MyTy> m_parent;
	std::vector<UserDataWrapper> m_userData;
};

} // namespace CoilCl
} // namespace AST

//
// Operator nodes
//

class Operator
	: public Returnable
	, public CoilCl::AST::ASTNode
{
	NODE_ID(AST::NodeID::OPERATOR_ID);

protected:
	SERIALIZE(ASTNode);
	DESERIALIZE(ASTNode);
	virtual ~Operator() = 0;
};

class BinaryOperator
	: public Operator
	, public SelfReference<BinaryOperator>
{
	NODE_ID(AST::NodeID::BINARY_OPERATOR_ID);
	std::shared_ptr<ASTNode> m_lhs;
	std::shared_ptr<ASTNode> m_rhs;

public:
	enum BinOperand
	{
		PLUS,		// +
		MINUS,		// -
		MUL,		// *
		DIV,		// /
		MOD,		// %
		ASSGN,		// =

		XOR,		// ^
		AND,		// &

		SLEFT,		// <<
		SRIGHT,		// >>

		EQ,			// ==
		NEQ,		// !=
		LT,			// <
		GT,			// >
		LE,			// <=
		GE,			// >=

		LAND,		// &&
		LOR,		// ||
	} m_operand;

	const char *BinOperandStr(BinOperand operand) const;

public:
	explicit BinaryOperator(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	BinaryOperator(BinOperand operand, const std::shared_ptr<ASTNode>& leftSide);

	void SetRightSide(const std::shared_ptr<ASTNode>& node);

	void Emplace(size_t idx, const std::shared_ptr<ASTNode>&& node) override;

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	const std::string NodeName() const;

private:
	POLY_IMPL();
};

class ConditionalOperator
	: public Operator
	, public SelfReference<ConditionalOperator>
{
	NODE_ID(AST::NodeID::CONDITIONAL_OPERATOR_ID);
	std::shared_ptr<ASTNode> evalNode;
	std::shared_ptr<ASTNode> truthStmt;
	std::shared_ptr<ASTNode> altStmt;

public:
	explicit ConditionalOperator(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	ConditionalOperator(std::shared_ptr<ASTNode>& eval, std::shared_ptr<ASTNode> truth = nullptr, std::shared_ptr<ASTNode> alt = nullptr);

	void SetTruthCompound(const std::shared_ptr<ASTNode>& node);
	void SetAltCompound(const std::shared_ptr<ASTNode>& node);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	virtual const std::string NodeName() const;

private:
	POLY_IMPL();
};

namespace CoilCl
{
namespace AST
{

class UnaryOperator
	: public Operator
	, public SelfReference<UnaryOperator>
{
	NODE_ID(AST::NodeID::UNARY_OPERATOR_ID);
	std::shared_ptr<ASTNode> m_body;

public:
	enum UnaryOperand
	{
		INC,		// ++
		DEC,		// --

		INTPOS,		// +
		INTNEG,		// -

		ADDR,		// &
		PTRVAL,		// *

		BITNOT,		// ~
		BOOLNOT,	// !
	};

	const char *UnaryOperandStr(UnaryOperand operand) const;

public:
	enum OperandSide
	{
		POSTFIX,
		PREFIX,
	};

public:
	explicit UnaryOperator(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	UnaryOperator(UnaryOperand operand, OperandSide side, const std::shared_ptr<ASTNode>& node);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	virtual NodeID Label() const noexcept override { return nodeId; }
	const std::string NodeName() const;

private:
	UnaryOperand m_operand;
	OperandSide m_side;
	POLY_IMPL();
};

} // namespace AST
} // namespace CoilCl

class CompoundAssignOperator
	: public Operator
	, public SelfReference<CompoundAssignOperator>
{
	NODE_ID(AST::NodeID::COMPOUND_ASSIGN_OPERATOR_ID);
	std::shared_ptr<ASTNode> m_body;
	std::shared_ptr<DeclRefExpr> m_identifier;

public:
	enum CompoundAssignOperand
	{
		MUL,		// *=
		DIV,		// /=
		MOD,		// %=
		ADD,		// +=
		SUB,		// -=
		LEFT,		// <<=
		RIGHT,		// >>=
		AND,		// &=
		XOR,		// ^=
		OR,			// |=
	};

	const char *CompoundAssignOperandStr(CompoundAssignOperand operand) const;

public:
	explicit CompoundAssignOperator(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	CompoundAssignOperator(CompoundAssignOperand operand, const std::shared_ptr<DeclRefExpr>& node);

	void SetRightSide(const std::shared_ptr<ASTNode>& node);
	
	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	const std::string NodeName() const;

private:
	CompoundAssignOperand m_operand;
	POLY_IMPL();
};

//
// Literal nodes
//

class Literal
	: public Returnable
	, public CoilCl::AST::ASTNode
{
	NODE_ID(AST::NodeID::LITERAL_ID);

protected:
	virtual ~Literal() = 0;

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	template<typename... _VariaTy>
	Literal(_VariaTy&&... args)
		: Returnable{ std::forward<_VariaTy>(args)... }
	{
	}
};

namespace Detail
{

template<typename _NativTy, class _DrivTy>
class LiteralImpl
	: public Literal
	, public SelfReference<_DrivTy>
{
protected:
	std::shared_ptr<CoilCl::Valuedef::ValueObject<_NativTy>> m_valueObj;

public:
	// Default to void type with no data
	// FUTURE: Make this more readable and only instantiate a single object
	LiteralImpl()
		: Literal{ AST::TypeFacade{ CoilCl::Util::MakeBuiltinType(CoilCl::Typedef::BuiltinType::Specifier::VOID_T) } }
		, m_valueObj{ CoilCl::Util::MakeBuiltinType(CoilCl::Typedef::BuiltinType::Specifier::VOID_T) }
	{
	}

	// Move data object from token processor into literal object
	LiteralImpl(std::shared_ptr<CoilCl::Valuedef::ValueObject<_NativTy>>&& object)
		: Literal{ AST::TypeFacade{ object->DataType() } }
		, m_valueObj{ std::move(object) }
	{
	}

	virtual void Serialize(Serializable::Interface& pack)
	{
		pack << _DrivTy::nodeId;

		Cry::ByteArray buffer = m_valueObj->Serialize();
		pack << buffer;

		Literal::Serialize(pack);
	}

	virtual void Deserialize(Serializable::Interface& pack)
	{
		AST::NodeID _nodeId;

		pack >> _nodeId;
		AssertNode(_nodeId, _DrivTy::nodeId);

		Cry::ByteArray buffer;
		pack >> buffer;
		//TODO: validate buffer
		m_valueObj = CoilCl::Util::ValueFactory::MakeValue<_NativTy>(buffer);

		Literal::Deserialize(pack);
	}

	const std::string NodeName() const
	{
		std::stringstream ss;
		ss << RemoveClassFromName(typeid(_DrivTy).name());
		ss << " {" + std::to_string(m_state.Alteration()) + "}";
		ss << " <line:" << line << ",col:" << col << "> ";
		ss << "'" << ReturnType()->TypeName() << "' ";
		ss << m_valueObj->Print();

		return ss.str();
	}

private:
	POLY_IMPL();
};

} // namespace Detail

class CharacterLiteral : public Detail::LiteralImpl<char, CharacterLiteral>
{
	NODE_ID(AST::NodeID::CHARACTER_LITERAL_ID);

	friend class Detail::LiteralImpl<char, CharacterLiteral>;

public:
	explicit CharacterLiteral(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	template<typename _Ty>
	CharacterLiteral(_Ty&& value)
		: LiteralImpl{ std::forward<_Ty>(value) }
	{
	}

	LABEL();
};

class StringLiteral : public Detail::LiteralImpl<std::string, StringLiteral>
{
	NODE_ID(AST::NodeID::STRING_LITERAL_ID);

	friend class Detail::LiteralImpl<std::string, StringLiteral>;

public:
	explicit StringLiteral(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	template<typename _Ty>
	StringLiteral(_Ty&& value)
		: LiteralImpl{ std::forward<_Ty>(value) }
	{
	}

	LABEL();
};

class IntegerLiteral : public Detail::LiteralImpl<int, IntegerLiteral>
{
	NODE_ID(AST::NodeID::INTEGER_LITERAL_ID);

	friend class Detail::LiteralImpl<int, IntegerLiteral>;

public:
	explicit IntegerLiteral(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	template<typename _Ty>
	IntegerLiteral(_Ty&& value)
		: LiteralImpl{ std::forward<_Ty>(value) }
	{
	}

	LABEL();
};

class FloatingLiteral : public Detail::LiteralImpl<double, FloatingLiteral>
{
	NODE_ID(AST::NodeID::FLOAT_LITERAL_ID);

	friend class Detail::LiteralImpl<double, FloatingLiteral>;

public:
	explicit FloatingLiteral(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	template<typename _Ty>
	FloatingLiteral(_Ty&& value)
		: LiteralImpl{ std::forward<_Ty>(value) }
	{
	}

	LABEL();
};

//
// Declaration nodes
//

class Decl
	: public Returnable
	, public CoilCl::AST::ASTNode
	, public CoilCl::AST::RefCount
{
	NODE_ID(AST::NodeID::DECL_ID);

protected:
	std::string m_identifier;

protected:
	virtual ~Decl() = 0;

	// Constructor only available for deserialization
	// operations. This constructor should never be called
	// direct, and is only available to derived classes.
	explicit Decl(Serializable::Interface&)
	{
	}

	// Default initializer with empty identifier
	Decl() = default;

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	//TODO: temp, remove afterwards
	Decl(const std::string& name)
		: m_identifier{ name }
	{
	}

	Decl(const std::string& name, const std::shared_ptr<Typedef::TypedefBase>& specifier)
		: Returnable{ AST::TypeFacade{ specifier } }
		, m_identifier{ name }
	{
	}

public:
	auto Identifier() const { return m_identifier; }
};

class VarDecl
	: public Decl
	, public SelfReference<VarDecl>
{
	NODE_ID(AST::NodeID::VAR_DECL_ID);
	std::shared_ptr<ASTNode> m_body;

public:
	explicit VarDecl(Serializable::Interface& pack)
		: Decl{ pack }
	{
		Deserialize(pack);
	}

	VarDecl(const std::string& name, std::shared_ptr<Typedef::TypedefBase> type, std::shared_ptr<ASTNode> node = nullptr);

	void Emplace(size_t idx, const std::shared_ptr<ASTNode>&& node) override;

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	virtual const std::string NodeName() const;

private:
	POLY_IMPL();
};

class ParamDecl
	: public Decl
	, public SelfReference<ParamDecl>
{
	NODE_ID(AST::NodeID::PARAM_DECL_ID);

public:
	explicit ParamDecl(Serializable::Interface& pack)
		: Decl{ pack }
	{
		Deserialize(pack);
	}

	ParamDecl(const std::string& name, std::shared_ptr<Typedef::TypedefBase> type)
		: Decl{ name, type }
	{
	}

	ParamDecl(const std::shared_ptr<Typedef::TypedefBase>& type)
		: Decl{ "", type }
	{
	}

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	virtual const std::string NodeName() const;

private:
	POLY_IMPL();
};

class VariadicDecl
	: public Decl
	, public SelfReference<VariadicDecl>
{
	NODE_ID(AST::NodeID::VARIADIC_DECL_ID);

public:
	explicit VariadicDecl(Serializable::Interface& pack)
		: Decl{ pack }
	{
		Deserialize(pack);
	}

	VariadicDecl()
		: Decl{ "", std::dynamic_pointer_cast<Typedef::TypedefBase>(Util::MakeVariadicType()) }
	{
	}

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	PRINT_NODE(VariadicDecl);

private:
	POLY_IMPL();
};

class TypedefDecl
	: public Decl
	, public SelfReference<TypedefDecl>
{
	NODE_ID(AST::NodeID::TYPEDEF_DECL_ID);

public:
	explicit TypedefDecl(Serializable::Interface& pack)
		: Decl{ pack }
	{
		Deserialize(pack);
	}

	TypedefDecl(const std::string& name, std::shared_ptr<Typedef::TypedefBase> type);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	virtual const std::string NodeName() const;

private:
	POLY_IMPL();
};

class FieldDecl
	: public Decl
	, public SelfReference<FieldDecl>
{
	NODE_ID(AST::NodeID::FIELD_DECL_ID);
	std::shared_ptr<IntegerLiteral> m_bits;

public:
	explicit FieldDecl(Serializable::Interface& pack)
		: Decl{ pack }
	{
		Deserialize(pack);
	}

	FieldDecl(const std::string& name, std::shared_ptr<Typedef::TypedefBase> type);

	void SetBitField(const std::shared_ptr<IntegerLiteral>& node);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	virtual const std::string NodeName() const;

private:
	POLY_IMPL();
};

class RecordDecl
	: public Decl
	, public SelfReference<RecordDecl>
{
	NODE_ID(AST::NodeID::RECORD_DECL_ID);
	std::vector<std::shared_ptr<FieldDecl>> m_fields;

public:
	enum RecordType
	{
		STRUCT,
		UNION,
	};

private:
	RecordType m_type = RecordType::STRUCT;

public:
	explicit RecordDecl(Serializable::Interface& pack)
		: Decl{ pack }
	{
		Deserialize(pack);
	}

	RecordDecl(const std::string& name);
	RecordDecl(RecordType type);

	bool IsAnonymous() const;

	void SetName(const std::string& name);

	void AddField(std::shared_ptr<FieldDecl>& node);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	virtual const std::string NodeName() const;

private:
	POLY_IMPL();
};

class EnumConstantDecl
	: public Decl
	, public SelfReference<EnumConstantDecl>
{
	NODE_ID(AST::NodeID::ENUM_CONSTANT_DECL_ID);
	std::shared_ptr<ASTNode> m_body;

public:
	explicit EnumConstantDecl(Serializable::Interface& pack)
		: Decl{ pack }
	{
		Deserialize(pack);
	}

	EnumConstantDecl(const std::string& name);

	void SetAssignment(std::shared_ptr<ASTNode>& node);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	const std::string NodeName() const;

private:
	POLY_IMPL();
};

class EnumDecl
	: public Decl
	, public SelfReference<EnumDecl>
{
	NODE_ID(AST::NodeID::ENUM_DECL_ID);
	std::vector<std::shared_ptr<EnumConstantDecl>> m_constants;

public:
	explicit EnumDecl(Serializable::Interface& pack)
		: Decl{ pack }
	{
		Deserialize(pack);
	}

	EnumDecl() = default;

	auto IsAnonymous() const;

	void SetName(const std::string& name);

	void AddConstant(std::shared_ptr<EnumConstantDecl>& node);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	const std::string NodeName() const;

private:
	POLY_IMPL();
};

class FunctionDecl
	: public Decl
	, public SelfReference<FunctionDecl>
{
	NODE_ID(AST::NodeID::FUNCTION_DECL_ID);
	std::shared_ptr<ParamStmt> m_params;
	std::shared_ptr<CompoundStmt> m_body;
	std::weak_ptr<FunctionDecl> m_protoRef;
	std::vector<AST::TypeFacade> m_signature;

	bool m_isPrototype = true;

#if 0
	size_t m_useCount = 0;
#endif

public:
	explicit FunctionDecl(Serializable::Interface& pack)
		: Decl{ pack }
	{
		Deserialize(pack);
	}

	FunctionDecl(const std::string& name, std::shared_ptr<CompoundStmt>& node);
	FunctionDecl(const std::string& name, std::shared_ptr<Typedef::TypedefBase> type);

	// If function declaration has a body, its not a prototype
	void SetCompound(const std::shared_ptr<CompoundStmt>& node);

	void SetSignature(std::vector<AST::TypeFacade>&& signature);

	void SetParameterStatement(const std::shared_ptr<ParamStmt>& node);

	auto HasSignature() const { return !m_signature.empty(); }
	auto& Signature() const { return m_signature; }
	auto& ParameterStatement() const { return m_params; }
	auto IsPrototypeDefinition() const { return m_isPrototype; }
	auto HasPrototypeDefinition() const { return !m_protoRef.expired(); }
	auto PrototypeDefinition() const { return m_protoRef.lock(); }

	// Bind function body to prototype definition
	void BindPrototype(const std::shared_ptr<FunctionDecl>& node);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	const std::string NodeName() const;

private:
	POLY_IMPL();
};

// The translation unit declaration cannot be stack instantiated
// and thus provides the declaration class a make command.
class TranslationUnitDecl
	: public Decl
	, public SelfReference<TranslationUnitDecl>
{
	NODE_ID(AST::NodeID::TRANSLATION_UNIT_DECL_ID);
	std::list<std::shared_ptr<ASTNode>> m_children;

private:
	TranslationUnitDecl(const std::string& sourceName)
		: Decl{ sourceName }
	{
	}

public:
	using NodeTrait = AST::Trait::RootNodeTag;

public:
	explicit TranslationUnitDecl(Serializable::Interface& pack)
		: Decl{ pack }
	{
		Deserialize(pack);
	}

	void AppendChild(const std::shared_ptr<ASTNode>& node) final;

	template<typename... _Args>
	static std::shared_ptr<TranslationUnitDecl> Make(_Args&&... args)
	{
		auto ptr = std::shared_ptr<TranslationUnitDecl>{ new TranslationUnitDecl{ std::forward<_Args>(args)... } };
		ptr->UpdateDelegate();
		return ptr;
	}

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	PRINT_NODE(TranslationUnitDecl);

private:
	POLY_IMPL();
};

//
// Expression nodes
//

class Expr
	: public Returnable
	, public CoilCl::AST::ASTNode
{
	NODE_ID(AST::NodeID::EXPR_ID);

protected:
	SERIALIZE(ASTNode);
	DESERIALIZE(ASTNode);
	virtual ~Expr() = 0;
};

class ResolveRefExpr
	: public Expr
	, public SelfReference<ResolveRefExpr>
{
	NODE_ID(AST::NodeID::RESOLVE_REF_EXPR_ID);
	std::string m_identifier;

public:
	ResolveRefExpr() = default;
	ResolveRefExpr(const std::string& identifier);

	//TODO: friend
	auto Identifier() const
	{
		return m_identifier;
	}

protected:
	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	const std::string NodeName() const;

private:
	POLY_IMPL();
};

class DeclRefExpr
	: public ResolveRefExpr
{
	NODE_ID(AST::NodeID::DECL_REF_EXPR_ID);
	std::weak_ptr<Decl> m_ref;

public:
	explicit DeclRefExpr(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	// We're not saving the reference as child in the root to prevent
	// circulair references in the upper node.
	explicit DeclRefExpr(std::shared_ptr<Decl>& ref)
		: m_ref{ ref }
	{
	}

	explicit DeclRefExpr(const std::string& identifier)
		: ResolveRefExpr{ identifier }
	{
	}

	//TODO: friend
	auto IsResolved() const { return !m_ref.expired(); }
	auto Reference() const { return m_ref.lock(); }

	void Resolve(const std::shared_ptr<ASTNode>& ref);

	// If reference resolves there is an return type
	bool HasReturnType() const override;

	// 
	void SetReturnType(AST::TypeFacade type) override;

	const AST::TypeFacade& ReturnType() const override;

	AST::TypeFacade& UpdateReturnType() override;

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	const std::string NodeName() const;

	std::shared_ptr<ASTNode> PolySelf()
	{
		return std::dynamic_pointer_cast<ASTNode>(ResolveRefExpr::GetSharedSelf());
	}
};

class CallExpr
	: public Expr
	, public SelfReference<CallExpr>
{
	NODE_ID(AST::NodeID::CALL_EXPR_ID);
	std::shared_ptr<DeclRefExpr> m_funcRef;
	std::shared_ptr<ArgumentStmt> m_args;

public:
	explicit CallExpr(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	CallExpr(std::shared_ptr<DeclRefExpr>& func, std::shared_ptr<ArgumentStmt> args = nullptr)
		//: Expr{} //TODO
	{
		ASTNode::AppendChild(NODE_UPCAST(func));
		m_funcRef = func;

		if (args != nullptr) {
			ASTNode::AppendChild(NODE_UPCAST(args));
			m_args = args;
		}
	}

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	const std::string NodeName() const;

	auto& ArgumentStatement() const { return m_args; }

	//TODO: friend
	auto FuncDeclRef() const
	{
		return m_funcRef;
	}

private:
	POLY_IMPL();
};

class BuiltinExpr final
	: public CallExpr
{
	NODE_ID(AST::NodeID::BUILTIN_EXPR_ID);
	std::shared_ptr<ASTNode> m_expr;
	AST::TypeFacade m_typenameType;

public:
	//TODO:?
	/*explicit BuiltinExpr(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}*/

	BuiltinExpr(std::shared_ptr<DeclRefExpr>& func, std::shared_ptr<DeclRefExpr> expr = nullptr, std::shared_ptr<ArgumentStmt> args = nullptr);

	void SetExpression(const std::shared_ptr<ASTNode>& node);

	//TODO: move?
	void SetTypename(std::shared_ptr<Typedef::TypedefBase>& type);

	///TODO: friends
	auto Expression() const { return m_expr; }
	auto TypeName() const { return m_typenameType; }

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	const std::string NodeName() const final;

	std::shared_ptr<ASTNode> PolySelf()
	{
		return std::dynamic_pointer_cast<ASTNode>(CallExpr::GetSharedSelf());
	}
};

class CastExpr
	: public Expr
	, public SelfReference<CastExpr>
{
	NODE_ID(AST::NodeID::CAST_EXPR_ID);
	std::shared_ptr<ASTNode> rtype;

public:
	explicit CastExpr(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	CastExpr(std::shared_ptr<ASTNode>& node)
		//: Expr{}
	{
		ASTNode::AppendChild(node);
		rtype = node;
	}

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	const std::string NodeName() const;

private:
	POLY_IMPL();
};

//TODO: converter function
class ImplicitConvertionExpr
	: public Expr
	, public SelfReference<ImplicitConvertionExpr>
{
	NODE_ID(AST::NodeID::IMPLICIT_CONVERTION_EXPR_ID);
	std::shared_ptr<ASTNode> m_body;
	Conv::Cast::Tag m_convOp;

public:
	explicit ImplicitConvertionExpr(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	ImplicitConvertionExpr(std::shared_ptr<ASTNode>& node, Conv::Cast::Tag convOp);

	virtual void Serialize(Serializable::Interface& pack)
	{
		pack << nodeId;

		auto group = pack.ChildGroups(1);
		group.Size(1);
		group << m_body;

		//pack << m_convOp;//TODO

		Expr::Serialize(pack);
	}

	virtual void Deserialize(Serializable::Interface& pack)
	{
		AST::NodeID _nodeId;
		pack >> _nodeId;
		AssertNode(_nodeId, nodeId);

		//pack >> m_convOp;//TODO

		auto group = pack.ChildGroups();
		pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
			m_body = node;
			ASTNode::AppendChild(node);
		}};

		Expr::Deserialize(pack);
	}

	LABEL();
	virtual const std::string NodeName() const;

private:
	POLY_IMPL();
};

class ParenExpr
	: public Expr
	, public SelfReference<ParenExpr>
{
	NODE_ID(AST::NodeID::PAREN_EXPR_ID);
	std::shared_ptr<ASTNode> m_body;

public:
	explicit ParenExpr(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	ParenExpr(std::shared_ptr<ASTNode>& node);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	PRINT_NODE(ParenExpr);

private:
	POLY_IMPL();
};

class InitListExpr
	: public Expr
	, public SelfReference<InitListExpr>
{
	NODE_ID(AST::NodeID::INIT_LIST_EXPR_ID);
	std::vector<std::shared_ptr<ASTNode>> m_children;

public:
	explicit InitListExpr(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	InitListExpr() = default;

	void AddListItem(const std::shared_ptr<ASTNode>& node);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	PRINT_NODE(InitListExpr);

private:
	POLY_IMPL();
};

class CompoundLiteralExpr
	: public Expr
	, public SelfReference<CompoundLiteralExpr>
{
	NODE_ID(AST::NodeID::COMPOUND_LITERAL_EXPR_ID);
	std::shared_ptr<InitListExpr> m_body;

public:
	explicit CompoundLiteralExpr(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	CompoundLiteralExpr(std::shared_ptr<InitListExpr>& node);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	PRINT_NODE(CompoundLiteralExpr);

private:
	POLY_IMPL();
};

class ArraySubscriptExpr
	: public Expr
	, public SelfReference<ArraySubscriptExpr>
{
	NODE_ID(AST::NodeID::ARRAY_SUBSCRIPT_EXPR_ID);
	std::shared_ptr<DeclRefExpr> m_identifier;
	std::shared_ptr<ASTNode> m_offset;

public:
	explicit ArraySubscriptExpr(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	ArraySubscriptExpr(std::shared_ptr<DeclRefExpr>& ref, std::shared_ptr<ASTNode>& expr);

	void Serialize(Serializable::Interface& pack);
	void Deserialize(Serializable::Interface& pack);

	LABEL();
	PRINT_NODE(ArraySubscriptExpr);

private:
	POLY_IMPL();
};

class MemberExpr
	: public Expr
	, public SelfReference<MemberExpr>
{
	NODE_ID(AST::NodeID::MEMBER_EXPR_ID);
	std::string m_name;
	std::shared_ptr<DeclRefExpr> m_record;

public:
	enum MemberType
	{
		REFERENCE,
		POINTER,
	} m_memberType;

public:
	explicit MemberExpr(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	MemberExpr(MemberType type, const std::string& name, std::shared_ptr<DeclRefExpr>& node);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	const std::string NodeName() const;

private:
	POLY_IMPL();
};

//
// Statement nodes
//

class Stmt : public CoilCl::AST::ASTNode
{
	NODE_ID(AST::NodeID::STMT_ID);

protected:
	SERIALIZE(ASTNode);
	DESERIALIZE(ASTNode);
	virtual ~Stmt() = 0;
};

class ContinueStmt
	: public Stmt
	, public SelfReference<ContinueStmt>
{
	NODE_ID(AST::NodeID::CONTINUE_STMT_ID);

public:
	explicit ContinueStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	ContinueStmt() = default;

	SERIALIZE(Stmt);
	DESERIALIZE(Stmt);
	LABEL();
	PRINT_NODE(ContinueStmt);

private:
	POLY_IMPL();
};

class ReturnStmt
	: public Stmt
	, public SelfReference<ReturnStmt>
{
	NODE_ID(AST::NodeID::RETURN_STMT_ID);
	std::shared_ptr<ASTNode> m_returnExpr;

public:
	explicit ReturnStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	ReturnStmt() = default;

	void SetReturnNode(std::shared_ptr<ASTNode>& node);

	auto HasExpression() const { return m_returnExpr != nullptr; }
	auto& Expression() const { return m_returnExpr; }

	void Emplace(size_t idx, const std::shared_ptr<ASTNode>&& node) override;

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	PRINT_NODE(ReturnStmt);

private:
	POLY_IMPL();
};

class IfStmt
	: public Stmt
	, public SelfReference<IfStmt>
{
	NODE_ID(AST::NodeID::IF_STMT_ID);
	std::shared_ptr<ASTNode> m_evalNode;
	std::shared_ptr<ASTNode> m_truthStmt;
	std::shared_ptr<ASTNode> m_altStmt;

public:
	explicit IfStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	IfStmt(std::shared_ptr<ASTNode>& eval, std::shared_ptr<ASTNode> truth = nullptr, std::shared_ptr<ASTNode> alt = nullptr);

	void SetTruthCompound(const std::shared_ptr<ASTNode>& node);
	void SetAltCompound(const std::shared_ptr<ASTNode>& node);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	virtual const std::string NodeName() const;

private:
	POLY_IMPL();
};

class SwitchStmt
	: public Stmt
	, public SelfReference<SwitchStmt>
{
	NODE_ID(AST::NodeID::SWITCH_STMT_ID);
	std::shared_ptr<ASTNode> evalNode;
	std::shared_ptr<ASTNode> m_body;

public:
	explicit SwitchStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	SwitchStmt(std::shared_ptr<ASTNode>& eval, std::shared_ptr<ASTNode> body = nullptr);

	void SetBody(const std::shared_ptr<ASTNode>& node);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	PRINT_NODE(SwitchStmt);

private:
	POLY_IMPL();
};

class WhileStmt
	: public Stmt
	, public SelfReference<WhileStmt>
{
	NODE_ID(AST::NodeID::WHILE_STMT_ID);
	std::shared_ptr<ASTNode> evalNode;
	std::shared_ptr<ASTNode> m_body;

public:
	explicit WhileStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	WhileStmt(std::shared_ptr<ASTNode>& eval, std::shared_ptr<ASTNode> body = nullptr);

	void SetBody(const std::shared_ptr<ASTNode>& node);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	PRINT_NODE(WhileStmt);

private:
	POLY_IMPL();
};

class DoStmt
	: public Stmt
	, public SelfReference<DoStmt>
{
	NODE_ID(AST::NodeID::DO_STMT_ID);
	std::shared_ptr<ASTNode> evalNode;
	std::shared_ptr<ASTNode> m_body;

public:
	explicit DoStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	DoStmt(std::shared_ptr<ASTNode>& body, std::shared_ptr<ASTNode> eval = nullptr);

	void SetEval(const std::shared_ptr<ASTNode>& node);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	PRINT_NODE(DoStmt);

private:
	POLY_IMPL();
};

class ForStmt
	: public Stmt
	, public SelfReference<ForStmt>
{
	NODE_ID(AST::NodeID::FOR_STMT_ID);
	std::shared_ptr<ASTNode> m_node1;
	std::shared_ptr<ASTNode> m_node2;
	std::shared_ptr<ASTNode> m_node3;
	std::shared_ptr<ASTNode> m_body;

public:
	explicit ForStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	ForStmt(std::shared_ptr<ASTNode>& node1, std::shared_ptr<ASTNode>& node2, std::shared_ptr<ASTNode>& node3);

	void SetBody(const std::shared_ptr<ASTNode>& node);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	PRINT_NODE(ForStmt);

private:
	POLY_IMPL();
};

class BreakStmt
	: public Stmt
	, public SelfReference<BreakStmt>
{
	NODE_ID(AST::NodeID::BREAK_STMT_ID);

public:
	explicit BreakStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	BreakStmt() = default;

	SERIALIZE(Stmt);
	DESERIALIZE(Stmt);

	LABEL();
	PRINT_NODE(BreakStmt);

private:
	POLY_IMPL();
};

class DefaultStmt
	: public Stmt
	, public SelfReference<DefaultStmt>
{
	NODE_ID(AST::NodeID::DEFAULT_STMT_ID);
	std::shared_ptr<ASTNode> m_body;

public:
	explicit DefaultStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	DefaultStmt(const std::shared_ptr<ASTNode>& body);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	PRINT_NODE(DefaultStmt);

private:
	POLY_IMPL();
};

class CaseStmt
	: public Stmt
	, public SelfReference<CaseStmt>
{
	NODE_ID(AST::NodeID::CASE_STMT_ID);
	std::shared_ptr<ASTNode> m_name;
	std::shared_ptr<ASTNode> m_body;

public:
	explicit CaseStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	CaseStmt(std::shared_ptr<ASTNode>& name, std::shared_ptr<ASTNode>& body);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	PRINT_NODE(CaseStmt);

private:
	POLY_IMPL();
};

class DeclStmt
	: public Stmt
	, public SelfReference<DeclStmt>
{
	NODE_ID(AST::NodeID::DECL_STMT_ID);
	std::list<std::shared_ptr<VarDecl>> m_var;

public:
	explicit DeclStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	DeclStmt() = default;

	void AddDeclaration(const std::shared_ptr<VarDecl>& node);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	PRINT_NODE(DeclStmt);

private:
	POLY_IMPL();
};

class ArgumentStmt
	: public Stmt
	, public SelfReference<ArgumentStmt>
{
	NODE_ID(AST::NodeID::ARGUMENT_STMT_ID);
	std::vector<std::shared_ptr<ASTNode>> m_arg;

public:
	explicit ArgumentStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	ArgumentStmt() = default;

	void AppendArgument(const std::shared_ptr<ASTNode>& node);

	void Emplace(size_t idx, const std::shared_ptr<ASTNode>&& node) override final;

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	PRINT_NODE(ArgumentStmt);

private:
	POLY_IMPL();
};

class ParamStmt
	: public Stmt
	, public SelfReference<ParamStmt>
{
	NODE_ID(AST::NodeID::PARAM_STMT_ID);
	std::vector<std::shared_ptr<ASTNode>> m_param;

public:
	explicit ParamStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	ParamStmt() = default;

	void AppendParamter(const std::shared_ptr<ASTNode>& node);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	PRINT_NODE(ParamStmt);

private:
	POLY_IMPL();
};

class LabelStmt
	: public Stmt
	, public SelfReference<LabelStmt>
{
	NODE_ID(AST::NodeID::LABEL_STMT_ID);
	std::string m_name;
	std::shared_ptr<ASTNode> m_body;

public:
	explicit LabelStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	LabelStmt(const std::string& name, std::shared_ptr<ASTNode>& node);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	PRINT_NODE(LabelStmt);

private:
	POLY_IMPL();
};

class GotoStmt
	: public Stmt
	, public SelfReference<GotoStmt>
{
	NODE_ID(AST::NodeID::GOTO_STMT_ID);
	std::string m_labelName;

public:
	explicit GotoStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	GotoStmt(const std::string& name);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	virtual const std::string NodeName() const;

private:
	POLY_IMPL();
};

class CompoundStmt
	: public Stmt
	, public SelfReference<CompoundStmt>
{
	NODE_ID(AST::NodeID::COMPOUND_STMT_ID);
	std::list<std::shared_ptr<ASTNode>> m_children;

public:
	CompoundStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	CompoundStmt() = default;

	void AppendChild(const std::shared_ptr<ASTNode>& node) final;

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	PRINT_NODE(CompoundStmt);

private:
	POLY_IMPL();
};
