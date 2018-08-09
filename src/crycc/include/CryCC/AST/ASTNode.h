// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/SourceLocation.h>

#include <CryCC/AST/RefCount.h>
#include <CryCC/AST/NodeId.h>
#include <CryCC/AST/NodeInterface.h>
#include <CryCC/AST/Unique.h>
#include <CryCC/AST/Serialize.h>
#include <CryCC/AST/ASTState.h>
#include <CryCC/AST/ASTTrait.h>

#include <CryCC/SubValue/UserData.h>
#include <CryCC/SubValue/TypeFacade.h>
#include <CryCC/SubValue/Converter.h>
#include <CryCC/SubValue/Valuedef.h>
#include <CryCC/SubValue/ValueHelper.h>

#include <Cry/Cry.h>
#include <Cry/Serialize.h>

#include <boost/any.hpp>

#include <vector>
#include <memory>
#include <sstream>

#define PRINT_NODE(n) \
	virtual const std::string NodeName() const \
	{ \
		return std::string{ RemoveClassFromName(typeid(n).name()) } + " {" + std::to_string(m_state.Alteration()) + "}" + " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + ">"; \
	}

#define NODE_UPCAST(c) \
	std::static_pointer_cast<ASTNode>(c)

#define POLY_IMPL() \
	ASTNodeType PolySelf() \
	{ \
		return this->GetSharedSelf(); \
	}

#define BUMP_STATE() \
	m_state.Bump((*this));

#define NODE_ID(i) \
	static const NodeID nodeId = i;

#define LABEL() \
	virtual NodeID Label() const noexcept override { return nodeId; }

#define SERIALIZE(p) \
	virtual void Serialize(Serializable::Interface& pack) \
	{ \
		pack << nodeId; p::Serialize(pack); \
	}

#define DESERIALIZE(p) \
	virtual void Deserialize(Serializable::Interface& pack) \
	{ \
		NodeID _nodeId; pack >> _nodeId; AssertNode(_nodeId, nodeId); p::Deserialize(pack); \
	}

namespace Typedef = CryCC::SubValue::Typedef;
namespace Valuedef = CryCC::SubValue::Valuedef;

template<typename ClassName>
constexpr std::string RemoveClassFromName(ClassName *_name)
{
	std::string className{ _name };
	auto const pos = className.find_last_of(':');
	return className.substr(pos + 1);
}

template<typename Node, typename BaseNode = CryCC::AST::ASTNode>
class SelfReference : public std::enable_shared_from_this<Node>
{
protected:
	std::shared_ptr<BaseNode> GetSharedSelf()
	{
		return this->shared_from_this();
	}
};

//
// Forward declarations.
//

namespace CryCC
{
namespace AST
{

class DeclRefExpr;
class CompoundStmt;
class ArgumentStmt;
class ParamStmt;

using ASTNodeType = std::shared_ptr<ASTNode>;

class ASTNode
	: public UniqueObj
	, public VisitorInterface
	, public ModifierInterface
	, virtual public Serializable
{
	NODE_ID(NodeID::AST_NODE_ID);

protected:
	//TODO: Replace with Util::SourceLocation
	mutable int line = -1;
	mutable int col = -1;

public:
	ASTNode() = default;
	ASTNode(int _line, int _col);

	bool operator==(const ASTNode&) const noexcept;

	//
	// Node modifiers.
	//

	inline size_t ChildrenCount() const noexcept { return children.size(); }
	inline size_t ModifierCount() const { return m_state.Alteration(); }

	virtual void Emplace(size_t idx, const ASTNodeType&& node)
	{
		CRY_UNUSED(idx);
		CRY_UNUSED(node);

		assert(0);
	}

	//
	// Source location operations.
	//

	// Set source location.
	void SetLocation(int, int) const;
	// Set source location as pair.
	void SetLocation(const std::pair<int, int>&) const;
	// Set source location as object.
	void SetLocation(CryCC::SourceLocation&&);
	// Get source location.
	std::pair<int, int> Location() const;

	//
	// Abstract function interfaces.
	//

	// Get node name.
	virtual const std::string NodeName() const = 0;
	// Copy self and cast as base node.
	virtual ASTNodeType PolySelf() = 0;

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
	// Node relations.
	//

	//TODO: friend
	// Forward the random access operator on the child node list.
	std::weak_ptr<ASTNode> operator[](int idx)
	{
		return children[idx];
	}

	//TODO: friend
	// Forward the random access operator on the child node list.
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
	const std::vector<std::weak_ptr<ASTNode>>& Children() const
	{
		return children;
	}

	template<typename UserType, typename = typename std::enable_if<std::is_pointer<_Ty>::value>::type>
	void AddUserData(UserType data)
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
	// Node serialization interfaces.
	//

	// Serialize base node.
	virtual void Serialize(Serializable::Interface& pack);
	// Deserialzie base node.
	virtual void Deserialize(Serializable::Interface& pack);

protected:
	virtual void AppendChild(const ASTNodeType& node)
	{
		children.push_back(node);
	}

	virtual void RemoveChild(size_t idx)
	{
		assert(idx < children.size());
		children.erase(children.begin() + idx);
	}

	void SetParent(const ASTNodeType&& node)
	{
		m_parent = node;
	}

protected:
	ASTState<ASTNode> m_state;
	std::vector<std::weak_ptr<ASTNode>> children;
	std::weak_ptr<ASTNode> m_parent;
	std::vector<CryCC::SubValue::UserDataWrapper> m_userData;
};

//
// Operator nodes.
//

class Operator
	: public Returnable
	, public ASTNode
{
	NODE_ID(NodeID::OPERATOR_ID);

protected:
	SERIALIZE(ASTNode);
	DESERIALIZE(ASTNode);
	virtual ~Operator() = 0;
};

class BinaryOperator
	: public Operator
	, public SelfReference<BinaryOperator>
{
	NODE_ID(NodeID::BINARY_OPERATOR_ID);
	ASTNodeType m_lhs;
	ASTNodeType m_rhs;

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
		OR,			// |
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
	BinaryOperator(BinOperand operand, const ASTNodeType& leftSide);

	explicit BinaryOperator(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	auto LHS() const noexcept { return m_lhs; }
	auto RHS() const noexcept { return m_rhs; }

	BinOperand Operand() const noexcept { return m_operand; };

	void SetRightSide(const ASTNodeType& node);

	void Emplace(size_t idx, const ASTNodeType&& node) override;

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
	NODE_ID(NodeID::CONDITIONAL_OPERATOR_ID);
	ASTNodeType m_evalNode;
	ASTNodeType m_truthStmt;
	ASTNodeType m_altStmt;

public:
	ConditionalOperator(ASTNodeType& eval, ASTNodeType truth = nullptr, ASTNodeType alt = nullptr);

	explicit ConditionalOperator(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	auto& Expression() const { return m_evalNode; }
	auto& TruthStatement() const { return m_truthStmt; }
	auto& AltStatement() const { return m_altStmt; }

	void SetTruthCompound(const ASTNodeType& node); //TODO: rename to ...Statement
	void SetAltCompound(const ASTNodeType& node);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	virtual const std::string NodeName() const;

private:
	POLY_IMPL();
};

class UnaryOperator
	: public Operator
	, public SelfReference<UnaryOperator>
{
	NODE_ID(NodeID::UNARY_OPERATOR_ID);
	ASTNodeType m_body;

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

	UnaryOperator(UnaryOperand operand, OperandSide side, const ASTNodeType& node);

	auto Expression() const noexcept { return m_body; }

	UnaryOperand Operand() const noexcept { return m_operand; };
	OperandSide OperationSide() const noexcept { return m_side; };

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	virtual NodeID Label() const noexcept override { return nodeId; }
	const std::string NodeName() const;

private:
	UnaryOperand m_operand;
	OperandSide m_side;
	POLY_IMPL();
};

class CompoundAssignOperator
	: public Operator
	, public SelfReference<CompoundAssignOperator>
{
	NODE_ID(NodeID::COMPOUND_ASSIGN_OPERATOR_ID);
	ASTNodeType m_body;
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

	CompoundAssignOperand Operand() const noexcept { return m_operand; };

	void SetRightSide(const ASTNodeType& node);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	const std::string NodeName() const;

private:
	CompoundAssignOperand m_operand;
	POLY_IMPL();
};

//
// Literal nodes.
//

class Literal
	: public Returnable
	, public ASTNode
{
	NODE_ID(NodeID::LITERAL_ID);

public:
	//virtual std::shared_ptr<CryCC::Valuedef::Value> Type2() const noexcept = 0; //TODO: rename
	const Valuedef::Value& Value() const noexcept
	{
		return m_value;
	}

	//TODO: protected?
	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

protected:
	virtual ~Literal() = 0;

	Literal(const Valuedef::Value& value)
		: Returnable{ value.Type() }
		, m_value{ value }
	{
	}

	Literal(Valuedef::Value&& value)
		: Returnable{ value.Type() }
		, m_value{ std::move(value) }
	{
	}

	explicit Literal(Serializable::Interface&)
		: m_value{ Util::MakeInt(0) } //TOOD: temporary fix
	{
	}

	template<NodeID NodeId>
	void SerializeImpl(Serializable::Interface& pack)
	{
		pack << NodeId;

		Cry::ByteArray buffer = m_value.Serialize();
		pack << buffer;

		Literal::Serialize(pack);
	}

	template<NodeID NodeId>
	void DeserializeImpl(Serializable::Interface& pack)
	{
		NodeID _nodeId;

		pack >> _nodeId;
		AssertNode(_nodeId, NodeId);

		Cry::ByteArray buffer;
		pack >> buffer;
		//TODO: validate buffer
		m_value = Util::ValueFactory::MakeValue(buffer);

		Literal::Deserialize(pack);
	}

	template<typename DerivedType>
	const std::string NodeNameImpl() const
	{
		std::stringstream ss;
		ss << RemoveClassFromName(typeid(DerivedType).name());
		ss << " {" + std::to_string(m_state.Alteration()) + "}";
		ss << " <line:" << line << ",col:" << col << "> ";
		ss << "'" << ReturnType()->TypeName() << "' ";
		ss << "\"" << m_value.Print() << "\"";

		return ss.str();
	}

	Valuedef::Value m_value;
};

class CharacterLiteral
	: public Literal
	, public SelfReference<CharacterLiteral>
{
	NODE_ID(NodeID::CHARACTER_LITERAL_ID);

public:
	explicit CharacterLiteral(Serializable::Interface& pack)
		: Literal{ pack }
	{
		Deserialize(pack);
	}

	CharacterLiteral(const Valuedef::Value& value)
		: Literal{ value }
	{
	}

	CharacterLiteral(Valuedef::Value&& value)
		: Literal{ std::move(value) }
	{
	}

	CharacterLiteral(char value)
		: Literal{ Util::MakeChar(value) }
	{
	}

	virtual void Serialize(Serializable::Interface& pack)
	{
		SerializeImpl<NodeID::CHARACTER_LITERAL_ID>(pack);
	}
	virtual void Deserialize(Serializable::Interface& pack)
	{
		DeserializeImpl<NodeID::CHARACTER_LITERAL_ID>(pack);
	}

	const std::string NodeName() const
	{
		return NodeNameImpl<CharacterLiteral>();
	}

	LABEL();

private:
	POLY_IMPL();
};

class StringLiteral
	: public Literal
	, public SelfReference<StringLiteral>
{
	NODE_ID(NodeID::STRING_LITERAL_ID);

public:
	explicit StringLiteral(Serializable::Interface& pack)
		: Literal{ pack }
	{
		Deserialize(pack);
	}

	StringLiteral(const Valuedef::Value& value)
		: Literal{ value }
	{
	}

	StringLiteral(Valuedef::Value&& value)
		: Literal{ std::move(value) }
	{
	}

	StringLiteral(const std::string& value)
		: Literal{ Util::MakeString(value) }
	{
	}

	virtual void Serialize(Serializable::Interface& pack)
	{
		SerializeImpl<NodeID::STRING_LITERAL_ID>(pack);
	}
	virtual void Deserialize(Serializable::Interface& pack)
	{
		DeserializeImpl<NodeID::STRING_LITERAL_ID>(pack);
	}

	const std::string NodeName() const
	{
		return NodeNameImpl<StringLiteral>();
	}

	LABEL();

private:
	POLY_IMPL();
};

class IntegerLiteral
	: public Literal
	, public SelfReference<IntegerLiteral>
{
	NODE_ID(NodeID::INTEGER_LITERAL_ID);

public:
	explicit IntegerLiteral(Serializable::Interface& pack)
		: Literal{ pack }
	{
		Deserialize(pack);
	}

	IntegerLiteral(const Valuedef::Value& value)
		: Literal{ value }
	{
	}

	IntegerLiteral(Valuedef::Value&& value)
		: Literal{ std::move(value) }
	{
	}

	IntegerLiteral(int value)
		: Literal{ Util::MakeInt(value) }
	{
	}

	virtual void Serialize(Serializable::Interface& pack)
	{
		SerializeImpl<NodeID::INTEGER_LITERAL_ID>(pack);
	}
	virtual void Deserialize(Serializable::Interface& pack)
	{
		DeserializeImpl<NodeID::INTEGER_LITERAL_ID>(pack);
	}

	const std::string NodeName() const
	{
		return NodeNameImpl<IntegerLiteral>();
	}

	LABEL();

private:
	POLY_IMPL();
};

class FloatingLiteral
	: public Literal
	, public SelfReference<FloatingLiteral>
{
	NODE_ID(NodeID::FLOAT_LITERAL_ID);

public:
	explicit FloatingLiteral(Serializable::Interface& pack)
		: Literal{ pack }
	{
		Deserialize(pack);
	}

	FloatingLiteral(const Valuedef::Value& value)
		: Literal{ value }
	{
	}

	FloatingLiteral(Valuedef::Value&& value)
		: Literal{ std::move(value) }
	{
	}

	FloatingLiteral(float value)
		: Literal{ Util::MakeFloat(value) }
	{
	}

	virtual void Serialize(Serializable::Interface& pack)
	{
		SerializeImpl<NodeID::FLOAT_LITERAL_ID>(pack);
	}
	virtual void Deserialize(Serializable::Interface& pack)
	{
		DeserializeImpl<NodeID::FLOAT_LITERAL_ID>(pack);
	}

	const std::string NodeName() const
	{
		return NodeNameImpl<FloatingLiteral>();
	}

	LABEL();

private:
	POLY_IMPL();
};

//
// Declaration nodes.
//

class Decl
	: public Returnable
	, public ASTNode
	, public RefCount
{
	NODE_ID(NodeID::DECL_ID);

protected:
	std::string m_identifier;

protected:
	virtual ~Decl() = 0;

	// Default initializer with empty identifier.
	Decl() = default;

	// Constructor only available for deserialization
	// operations. This constructor should never be called
	// direct, and is only available to derived classes.
	explicit Decl(Serializable::Interface&)
	{
	}

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	//TODO: temp, remove afterwards
	Decl(const std::string& name)
		: m_identifier{ name }
	{
	}

	Decl(const std::string& name, const std::shared_ptr<Typedef::TypedefBase>& specifier)
		: Returnable{ Typedef::TypeFacade{ specifier } }
		, m_identifier{ name }
	{
	}

public:
	std::string Identifier() const noexcept { return m_identifier; }
};

class VarDecl
	: public Decl
	, public SelfReference<VarDecl>
{
	NODE_ID(NodeID::VAR_DECL_ID);
	ASTNodeType m_body;

public:
	explicit VarDecl(Serializable::Interface& pack)
		: Decl{ pack }
	{
		Deserialize(pack);
	}

	VarDecl(const std::string& name, std::shared_ptr<Typedef::TypedefBase> type, ASTNodeType node = nullptr);

	bool HasExpression() const { return m_body != nullptr; }
	auto& Expression() const { return m_body; }

	void Emplace(size_t idx, const ASTNodeType&& node) override;

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
	NODE_ID(NodeID::PARAM_DECL_ID);

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
	NODE_ID(NodeID::VARIADIC_DECL_ID);

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
	NODE_ID(NodeID::TYPEDEF_DECL_ID);

public:
	TypedefDecl(const std::string& name, std::shared_ptr<Typedef::TypedefBase> type);

	explicit TypedefDecl(Serializable::Interface& pack)
		: Decl{ pack }
	{
		Deserialize(pack);
	}

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
	NODE_ID(NodeID::FIELD_DECL_ID);
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
	NODE_ID(NodeID::RECORD_DECL_ID);
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

	bool HasFields() const noexcept { return !m_fields.empty(); }
	auto& Fields() const { return m_fields; }

	bool IsAnonymous() const;

	void SetName(const std::string& name);

	void AddField(std::shared_ptr<FieldDecl>& node);

	RecordType Type() const;

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
	NODE_ID(NodeID::ENUM_CONSTANT_DECL_ID);
	ASTNodeType m_body;

public:
	explicit EnumConstantDecl(Serializable::Interface& pack)
		: Decl{ pack }
	{
		Deserialize(pack);
	}

	EnumConstantDecl(const std::string& name);

	void SetAssignment(ASTNodeType& node);

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
	NODE_ID(NodeID::ENUM_DECL_ID);
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
	NODE_ID(NodeID::FUNCTION_DECL_ID);
	std::shared_ptr<ParamStmt> m_params;
	std::shared_ptr<CompoundStmt> m_body;
	std::weak_ptr<FunctionDecl> m_protoRef;
	std::vector<Typedef::TypeFacade> m_signature;

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
	void SetSignature(std::vector<Typedef::TypeFacade>&& signature);
	void SetParameterStatement(const std::shared_ptr<ParamStmt>& node);

	bool HasSignature() const noexcept { return !m_signature.empty(); }
	auto& Signature() const { return m_signature; }
	bool HasParameters() const noexcept { return m_params != nullptr; }
	auto& ParameterStatement() const { return m_params; }
	bool IsPrototypeDefinition() const noexcept { return m_isPrototype; }
	bool HasPrototypeDefinition() const noexcept { return !m_protoRef.expired(); }
	auto PrototypeDefinition() const { return m_protoRef.lock(); }
	auto& FunctionCompound() const { return m_body; }

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
	NODE_ID(NodeID::TRANSLATION_UNIT_DECL_ID);
	std::list<ASTNodeType> m_children;

public:
	using NodeTrait = Trait::RootNodeTag;

public:
	TranslationUnitDecl(const std::string& sourceName)
		: Decl{ sourceName }
	{
	}

	explicit TranslationUnitDecl(Serializable::Interface& pack)
		: Decl{ pack }
	{
		Deserialize(pack);
	}

	void AppendChild(const ASTNodeType&) final;

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
	, public ASTNode
{
	NODE_ID(NodeID::EXPR_ID);

protected:
	SERIALIZE(ASTNode);
	DESERIALIZE(ASTNode);
	virtual ~Expr() = 0;
};

class ResolveRefExpr
	: public Expr
	, public SelfReference<ResolveRefExpr>
{
	NODE_ID(NodeID::RESOLVE_REF_EXPR_ID);
	std::string m_identifier;

public:
	ResolveRefExpr() = default;
	ResolveRefExpr(const std::string& identifier);

	//TODO: friend
	std::string Identifier() const noexcept { return m_identifier; }

protected:
	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	const std::string NodeName() const;

private:
	POLY_IMPL();
};

//TODO: DeclRefExpr to ReferenceExpr
class DeclRefExpr
	: public ResolveRefExpr
{
	NODE_ID(NodeID::DECL_REF_EXPR_ID);
	std::weak_ptr<Decl> m_ref; //TODO: expand to AST::ASTNode

public:
	explicit DeclRefExpr(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	// NOTE: We're not saving the reference as child in the root
	// to prevent circulair references in the upper node.
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

	void Resolve(const ASTNodeType& ref);

	// If reference resolves there is an return type
	bool HasReturnType() const override;

	// 
	void SetReturnType(Typedef::TypeFacade type) override;

	const Typedef::TypeFacade& ReturnType() const override;

	Typedef::TypeFacade& UpdateReturnType() override;

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	const std::string NodeName() const;

	ASTNodeType PolySelf()
	{
		return std::dynamic_pointer_cast<ASTNode>(ResolveRefExpr::GetSharedSelf());
	}
};

class CallExpr
	: public Expr
	, public SelfReference<CallExpr>
{
	NODE_ID(NodeID::CALL_EXPR_ID);
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

		if (args) {
			ASTNode::AppendChild(NODE_UPCAST(args));
			m_args = args;
		}
	}

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	const std::string NodeName() const;

	auto& FunctionReference() const { return m_funcRef; }
	bool HasArguments() const noexcept { return m_args != nullptr; }
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
	NODE_ID(NodeID::BUILTIN_EXPR_ID);
	ASTNodeType m_body;
	Typedef::TypeFacade m_typenameType;

public:
	//TODO:?
	/*explicit BuiltinExpr(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}*/

	BuiltinExpr(std::shared_ptr<DeclRefExpr>& func, std::shared_ptr<DeclRefExpr> expr = nullptr, std::shared_ptr<ArgumentStmt> args = nullptr);

	void SetExpression(const ASTNodeType& node);

	//TODO: move?
	void SetTypename(std::shared_ptr<Typedef::TypedefBase>& type);

	///TODO: friends
	bool HasExpression() const { return m_body != nullptr; }
	auto Expression() const { return m_body; }
	auto TypeName() const { return m_typenameType; }

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	const std::string NodeName() const final;

	ASTNodeType PolySelf()
	{
		return std::dynamic_pointer_cast<ASTNode>(CallExpr::GetSharedSelf());
	}
};

class Convertible
{
	CryCC::SubValue::Conv::Cast::Tag m_convOp{ CryCC::SubValue::Conv::Cast::Tag::NONE_CAST };

protected:
	Convertible() = default;

	// Initialize object with converter.
	Convertible(CryCC::SubValue::Conv::Cast::Tag op)
		: m_convOp{ op }
	{
	}

public:
	virtual void SetConverterOperation(CryCC::SubValue::Conv::Cast::Tag op) { m_convOp = op; }
	virtual const CryCC::SubValue::Conv::Cast::Tag Converter() const { return m_convOp; }
};

class CastExpr
	: public Expr
	, public Convertible
	, public SelfReference<CastExpr>
{
	NODE_ID(NodeID::CAST_EXPR_ID);
	ASTNodeType m_body;

public:
	explicit CastExpr(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	CastExpr(ASTNodeType& node, std::shared_ptr<Typedef::TypedefBase> type);

	auto& Expression() const { return m_body; }

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	const std::string NodeName() const;

private:
	POLY_IMPL();
};

//TODO: converter function
//TODO: rename ImplicitCastExpr
class ImplicitConvertionExpr
	: public Expr
	, public Convertible
	, public SelfReference<ImplicitConvertionExpr>
{
	NODE_ID(NodeID::IMPLICIT_CONVERTION_EXPR_ID);
	ASTNodeType m_body;

public:
	explicit ImplicitConvertionExpr(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	ImplicitConvertionExpr(ASTNodeType& node, CryCC::SubValue::Conv::Cast::Tag convOp);

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	virtual const std::string NodeName() const;

private:
	POLY_IMPL();
};

class ParenExpr
	: public Expr
	, public SelfReference<ParenExpr>
{
	NODE_ID(NodeID::PAREN_EXPR_ID);
	ASTNodeType m_body;

public:
	explicit ParenExpr(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	ParenExpr(ASTNodeType& node);

	bool HasExpression() const { return m_body != nullptr; }
	auto& Expression() const { return m_body; }

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	virtual const std::string NodeName() const;

private:
	POLY_IMPL();
};

class InitListExpr
	: public Expr
	, public SelfReference<InitListExpr>
{
	NODE_ID(NodeID::INIT_LIST_EXPR_ID);
	std::vector<ASTNodeType> m_children;

public:
	explicit InitListExpr(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	InitListExpr() = default;

	std::vector<ASTNodeType> List() const noexcept;

	void AddListItem(const ASTNodeType& node);

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
	NODE_ID(NodeID::COMPOUND_LITERAL_EXPR_ID);
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
	NODE_ID(NodeID::ARRAY_SUBSCRIPT_EXPR_ID);
	std::shared_ptr<DeclRefExpr> m_identifier;
	ASTNodeType m_offset;

public:
	explicit ArraySubscriptExpr(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	ArraySubscriptExpr(std::shared_ptr<DeclRefExpr>& ref, ASTNodeType& expr);

	ASTNodeType OffsetExpression() const noexcept;

	std::shared_ptr<DeclRefExpr> ArrayDeclaration() const noexcept;

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
	NODE_ID(NodeID::MEMBER_EXPR_ID);
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

	std::string FieldName();

	std::shared_ptr<DeclRefExpr> RecordRef();

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

class Stmt : public ASTNode
{
	NODE_ID(NodeID::STMT_ID);

protected:
	SERIALIZE(ASTNode);
	DESERIALIZE(ASTNode);
	virtual ~Stmt() = 0;
};

class ContinueStmt
	: public Stmt
	, public SelfReference<ContinueStmt>
{
	NODE_ID(NodeID::CONTINUE_STMT_ID);

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
	NODE_ID(NodeID::RETURN_STMT_ID);
	ASTNodeType m_returnExpr;

public:
	explicit ReturnStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	ReturnStmt() = default;

	void SetReturnNode(ASTNodeType& node);

	auto HasExpression() const { return m_returnExpr != nullptr; }
	auto& Expression() const { return m_returnExpr; }

	void Emplace(size_t idx, const ASTNodeType&& node) override;

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
	NODE_ID(NodeID::IF_STMT_ID);
	ASTNodeType m_evalNode;
	ASTNodeType m_truthStmt;
	ASTNodeType m_altStmt;

public:
	IfStmt(ASTNodeType& eval, ASTNodeType truth = nullptr, ASTNodeType alt = nullptr);

	explicit IfStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	auto& Expression() const { return m_evalNode; }
	bool HasTruthCompound() const { return m_truthStmt != nullptr; }
	auto& TruthCompound() const { return m_truthStmt; }
	bool HasAltCompound() const { return m_altStmt != nullptr; }
	auto& AltCompound() const { return m_altStmt; }

	void SetTruthCompound(const ASTNodeType& node);
	void SetAltCompound(const ASTNodeType& node);

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
	NODE_ID(NodeID::SWITCH_STMT_ID);
	ASTNodeType evalNode;
	ASTNodeType m_body;

public:
	SwitchStmt(ASTNodeType& eval, ASTNodeType body = nullptr);

	explicit SwitchStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	auto& Expression() const { return evalNode; }
	bool HasBodyExpression() const { return m_body != nullptr; }
	auto& BodyExpression() const { return m_body; }

	void SetBody(const ASTNodeType& node);

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
	NODE_ID(NodeID::WHILE_STMT_ID);
	ASTNodeType evalNode;
	ASTNodeType m_body;

public:
	WhileStmt(ASTNodeType& eval, ASTNodeType body = nullptr);

	explicit WhileStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	auto& Expression() const { return evalNode; }
	bool HasBodyExpression() const { return m_body != nullptr; }
	auto& BodyExpression() const { return m_body; }

	void SetBody(const ASTNodeType& node);

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
	NODE_ID(NodeID::DO_STMT_ID);
	ASTNodeType evalNode;
	ASTNodeType m_body;

public:
	DoStmt(ASTNodeType& body, ASTNodeType eval = nullptr);

	explicit DoStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	auto& Expression() const { return evalNode; }
	bool HasBodyExpression() const { return m_body != nullptr; }
	auto& BodyExpression() const { return m_body; }

	void SetEval(const ASTNodeType& node);

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
	NODE_ID(NodeID::FOR_STMT_ID);
	ASTNodeType m_node1;
	ASTNodeType m_node2;
	ASTNodeType m_node3;
	ASTNodeType m_body;

public:
	ForStmt(ASTNodeType& node1, ASTNodeType& node2, ASTNodeType& node3);

	explicit ForStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	auto& Declaration() const { return m_node1; }
	auto& Expression() const { return m_node2; }
	auto& FinishStatement() const { return m_node3; }
	bool HasBodyExpression() const { return m_body != nullptr; }
	auto& BodyExpression() const { return m_body; }

	void SetBody(const ASTNodeType& node);

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
	NODE_ID(NodeID::BREAK_STMT_ID);

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
	NODE_ID(NodeID::DEFAULT_STMT_ID);
	ASTNodeType m_body;

public:
	explicit DefaultStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	DefaultStmt(const ASTNodeType& body);

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
	NODE_ID(NodeID::CASE_STMT_ID);
	ASTNodeType m_identifier;
	ASTNodeType m_body;

public:
	CaseStmt(ASTNodeType& name, ASTNodeType& body);

	explicit CaseStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	auto& Identifier() const noexcept { return m_identifier; }

	auto& Expression() const { return m_body; }

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
	NODE_ID(NodeID::DECL_STMT_ID);
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
	NODE_ID(NodeID::ARGUMENT_STMT_ID);
	std::vector<ASTNodeType> m_arg;

public:
	ArgumentStmt() = default;

	explicit ArgumentStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	void AppendArgument(const ASTNodeType& node);

	void Emplace(size_t idx, const ASTNodeType&& node) override final;

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
	NODE_ID(NodeID::PARAM_STMT_ID);
	std::vector<ASTNodeType> m_param; //TODO: vector of paramDecl?

public:
	ParamStmt() = default;

	explicit ParamStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	void AppendParamter(const ASTNodeType& node);

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
	NODE_ID(NodeID::LABEL_STMT_ID);
	std::string m_name;
	ASTNodeType m_body;

public:
	explicit LabelStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	LabelStmt(const std::string& name, ASTNodeType& node);

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
	NODE_ID(NodeID::GOTO_STMT_ID);
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
	NODE_ID(NodeID::COMPOUND_STMT_ID);
	std::list<ASTNodeType> m_children;

public:
	CompoundStmt() = default;

	explicit CompoundStmt(Serializable::Interface& pack)
	{
		Deserialize(pack);
	}

	void AppendChild(const ASTNodeType& node) final;

	virtual void Serialize(Serializable::Interface& pack);
	virtual void Deserialize(Serializable::Interface& pack);

	LABEL();
	PRINT_NODE(CompoundStmt);

private:
	POLY_IMPL();
};

} // namespace AST
} // namespace CryCC
