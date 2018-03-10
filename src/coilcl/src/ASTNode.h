// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/Indep.h>

#include "Typedef.h" //TODO: remove ?
#include "Valuedef.h" //TODO: remove ?
#include "TypeFacade.h"
#include "Converter.h"
#include "RefCount.h"
#include "NodeId.h"
#include "ASTState.h"
#include "UserData.h"

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
		return std::dynamic_pointer_cast<ASTNode>(this->GetSharedSelf()); \
	}

#define BUMP_STATE() \
	m_state.Bump((*this));

#define NODE_ID(i) \
	static const AST::NodeID nodeId = i;

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

class ASTNode;

template <typename _Ty>
struct Identity { using type = typename _Ty::value_type; };

template<typename _Ty, typename _Base = ASTNode>
class SelfReference : public std::enable_shared_from_this<_Ty>
{
protected:
	std::shared_ptr<_Base> GetSharedSelf()
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
	UniqueObj()
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

public:
	Returnable() = default;
	Returnable(AST::TypeFacade type)
		: m_returnType{ type }
	{
	}

	virtual bool HasReturnType() const { return m_returnType.HasValue(); }
	virtual void SetReturnType(AST::TypeFacade type) { m_returnType = type; }
	virtual AST::TypeFacade ReturnType() const { return m_returnType; }
};

struct Serializable
{
	struct Interface
	{
		virtual void SetId(int id) = 0;

		// Stream out operators
		virtual void operator<<(int i) = 0;
		virtual void operator<<(double d) = 0;
		virtual void operator<<(AST::NodeID n) = 0;
		virtual void operator<<(std::string s) = 0;

		// Stream in operators
		virtual void operator>>(int& i) = 0;
		virtual void operator>>(double& d) = 0;
		virtual void operator>>(AST::NodeID& n) = 0;
		virtual void operator>>(std::string& s) = 0;
	};

	virtual void Serialize(Interface&) = 0;
	virtual void Deserialize(Interface&) = 0;

protected:
	void AssertNode(const AST::NodeID& got, const AST::NodeID& exp) {
		if (got != exp) {
			throw 2; //TODO
		}
	}
};

struct ModifierInterface
{
	virtual void Emplace(size_t, const std::shared_ptr<ASTNode>&&) = 0;
	virtual size_t ModifierCount() const = 0;
};

class DeclRefExpr;
class CompoundStmt;
class ArgumentStmt;
class ParamStmt;

class ASTNode
	: public ModifierInterface
	, public UniqueObj
	, virtual public Serializable
{
	NODE_ID(AST::NodeID::AST_NODE_ID);

protected:
	using _MyTy = ASTNode;

protected:
	mutable int line = -1;
	mutable int col = -1;

public:
	ASTNode() = default;
	ASTNode(int _line, int _col)
		: line{ _line }
		, col{ _col }
	{
	}

	inline size_t ChildrenCount() const { return children.size(); }
	inline size_t ModifierCount() const { return m_state.Alteration(); }

	[[noreturn]]
	virtual void Emplace(size_t idx, const std::shared_ptr<ASTNode>&& node)
	{
		CRY_UNUSED(idx);
		CRY_UNUSED(node);

		assert(0);
	}

	void SetLocation(int _line, int _col) const
	{
		line = _line;
		col = _col;
	}

	void SetLocation(const std::pair<int, int>& loc) const
	{
		line = loc.first;
		col = loc.second;
	}

	std::pair<int, int> Location() const
	{
		return { line, col };
	}

	// Abstract function interfaces
	virtual const std::string NodeName() const = 0;
	virtual std::shared_ptr<ASTNode> PolySelf() = 0;

	enum struct Traverse
	{
		STAGE_FIRST = 0,
		STAGE_LAST = -1
	};

	template<Traverse _Ver>
	inline void Print() { this->Print(static_cast<int>(_Ver)); }

	//TODO: replace with algorithm ?
	void Print(int version, int level = 0, bool last = 0, std::vector<int> ignore = {}) const;

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
	std::vector<std::weak_ptr<ASTNode>>& Children()
	{
		return children;
	}

	template<typename _Ty, typename = std::enable_if<std::is_pointer<_Ty>::value>::type>
	void AddUserData(_Ty data)
	{
		m_userData.emplace_back(data);
	}

	/*template<typename _Pred>
	auto UserData(_Pred predicate)
	{
		return std::find_if(m_userData.begin(), m_userData.end(), predicate);
	}*/

	//TODO: friend
	void UpdateDelegate()
	{
		for (auto& wPtr : children) {
			if (auto ptr = wPtr.lock()) {
				ptr->SetParent(std::move(this->PolySelf()));
			}
		}
	}

	virtual void Serialize(Serializable::Interface& pack)
	{
		pack.SetId(UniqueObj::Id());
		pack << nodeId;
		pack << line;
		pack << col;

		/*for (const auto& data : m_userData) {
			pack << data->Serialize();
		}*/
	}

	virtual void Deserialize(Serializable::Interface& pack)
	{
		AST::NodeID _nodeId;

		pack >> _nodeId;
		AssertNode(_nodeId, nodeId);

		pack >> line;
		pack >> col;

		//TODO: deserialize user data
	}

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

//
// Operator nodes
//

class Operator
	: public Returnable
	, public ASTNode
{
	NODE_ID(AST::NodeID::OPERATOR_ID);

public:
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

	const char *BinOperandStr(BinOperand operand) const
	{
		switch (operand) {
		case BinOperand::PLUS:
			return "+";
		case BinOperand::MINUS:
			return "-";
		case BinOperand::MUL:
			return "*";
		case BinOperand::DIV:
			return "/";
		case BinOperand::MOD:
			return "%";
		case BinOperand::ASSGN:
			return "=";
		case BinOperand::XOR:
			return "^";
		case BinOperand::AND:
			return "&";
		case BinOperand::SLEFT:
			return "<<";
		case BinOperand::SRIGHT:
			return ">>";
		case BinOperand::EQ:
			return "==";
		case BinOperand::NEQ:
			return "!=";
		case BinOperand::LT:
			return "<";
		case BinOperand::GT:
			return ">";
		case BinOperand::LE:
			return "<=";
		case BinOperand::GE:
			return ">=";
		case BinOperand::LAND:
			return "&&";
		case BinOperand::LOR:
			return "||";
		}

		return "<unknown>";
	}

public:
	BinaryOperator(BinOperand operand, const std::shared_ptr<ASTNode>& leftSide)
		: m_operand{ operand }
		, m_lhs{ leftSide }
	{
		ASTNode::AppendChild(leftSide);
	}

	void SetRightSide(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		m_rhs = node;

		ASTNode::UpdateDelegate();
	}

	void Emplace(size_t idx, const std::shared_ptr<ASTNode>&& node) override
	{
		assert(idx == 0 || idx == 1);
		BUMP_STATE();

		ASTNode::RemoveChild(idx);
		ASTNode::AppendChild(node);

		if (idx == 0) {
			m_lhs = std::move(node);
		}
		else {
			m_rhs = std::move(node);
		}

		ASTNode::UpdateDelegate();
	}

	const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(BinaryOperator).name()) };
		_node += " {" + std::to_string(m_state.Alteration()) + "}";
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";

		if (Operator::ReturnType().HasValue()) {
			_node += "'" + Operator::ReturnType().TypeName() + "' ";
			_node += Operator::ReturnType()->StorageClassName();
		}

		_node += "'" + std::string{ BinOperandStr(m_operand) } +"'";

		return _node;
	}

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
	ConditionalOperator(std::shared_ptr<ASTNode>& eval, std::shared_ptr<ASTNode> truth = nullptr, std::shared_ptr<ASTNode> alt = nullptr)
		: evalNode{ eval }
	{
		ASTNode::AppendChild(eval);

		if (truth) {
			ASTNode::AppendChild(truth);
			truthStmt = truth;
		}

		if (alt) {
			ASTNode::AppendChild(alt);
			altStmt = alt;
		}
	}

	void SetTruthCompound(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		truthStmt = node;

		ASTNode::UpdateDelegate();
	}

	void SetAltCompound(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		altStmt = node;

		ASTNode::UpdateDelegate();
	}

	virtual const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(ConditionalOperator).name()) };
		_node += " {" + std::to_string(m_state.Alteration()) + "}";
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";

		return _node;
	}

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
	} m_operand;

	const char *UnaryOperandStr(UnaryOperand operand) const
	{
		switch (operand) {
		case UnaryOperand::INC:
			return "++";
		case UnaryOperand::DEC:
			return "--";
		case UnaryOperand::INTPOS:
			return "+";
		case UnaryOperand::INTNEG:
			return "-";
		case UnaryOperand::ADDR:
			return "&";
		case UnaryOperand::PTRVAL:
			return "*";
		case UnaryOperand::BITNOT:
			return "~";
		case UnaryOperand::BOOLNOT:
			return "!";
		}

		return "<unknown>";
	}

public:
	enum OperandSide
	{
		POSTFIX,
		PREFIX,
	} m_side;

public:
	UnaryOperator(UnaryOperand operand, OperandSide side, const std::shared_ptr<ASTNode>& node)
		: m_operand{ operand }
		, m_side{ side }
	{
		ASTNode::AppendChild(node);
		m_body = node;
	}

	const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(UnaryOperator).name()) };
		_node += +" {" + std::to_string(m_state.Alteration()) + "}";
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";

		switch (m_side) {
		case UnaryOperator::POSTFIX:
			_node += "postfix ";
			break;
		case UnaryOperator::PREFIX:
			_node += "prefix ";
			break;
		}

		_node += "'";
		_node += UnaryOperandStr(m_operand);
		_node += "'";
		return _node;
	}

private:
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
	} m_operand;

	const char *CompoundAssignOperandStr(CompoundAssignOperand operand) const
	{
		switch (operand) {
		case CompoundAssignOperand::MUL:
			return "*=";
		case CompoundAssignOperand::DIV:
			return "/=";
		case CompoundAssignOperand::MOD:
			return "%=";
		case CompoundAssignOperand::ADD:
			return "+=";
		case CompoundAssignOperand::SUB:
			return "-=";
		case CompoundAssignOperand::LEFT:
			return "<<=";
		case CompoundAssignOperand::RIGHT:
			return ">>=";
		case CompoundAssignOperand::AND:
			return "&=";
		case CompoundAssignOperand::XOR:
			return "^=";
		case CompoundAssignOperand::OR:
			return "|=";
		}

		return "<unknown>";
	}

public:
	CompoundAssignOperator(CompoundAssignOperand operand, const std::shared_ptr<DeclRefExpr>& node)
		: m_operand{ operand }
	{
		ASTNode::AppendChild(NODE_UPCAST(node));
		m_identifier = node;
	}

	void SetRightSide(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		m_body = node;

		ASTNode::UpdateDelegate();
	}

	const std::string NodeName() const
	{
		return std::string{ RemoveClassFromName(typeid(CompoundAssignOperator).name()) } +" {" + std::to_string(m_state.Alteration()) + "}" + " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> '" + CompoundAssignOperandStr(m_operand) + "'";
	}

private:
	POLY_IMPL();
};

//
// Literal nodes
//

class Literal
	: public Returnable
	, public ASTNode
{
	NODE_ID(AST::NodeID::LITERAL_ID);

public:
	virtual ~Literal() = 0;

	Literal() = default;

	template<typename... _VariaTy>
	Literal(_VariaTy&&... args)
		: Returnable{ std::forward<_VariaTy>(args)... }
	{
	}
};

template<typename _NativTy, class _DrivTy>
class LiteralImpl
	: public Literal
	, public SelfReference<_DrivTy>
{
protected:
	std::shared_ptr<CoilCl::Valuedef::ValueObject<_NativTy>> m_valueObj;

public:
	// Default to void type with no data
	LiteralImpl()
		: LiteralImpl{ CoilCl::Util::MakeBuiltinType(CoilCl::Typedef::BuiltinType::Specifier::VOID) }
	{
	}

	// Move data object from lexer into literal
	LiteralImpl(std::shared_ptr<CoilCl::Valuedef::ValueObject<_NativTy>>&& object)
		: Literal{ AST::TypeFacade{ object->DataType() } }
		, m_valueObj{ std::move(object) }
	{
	}

	const std::string NodeName() const
	{
		std::stringstream ss;
		ss << RemoveClassFromName(typeid(_DrivTy).name());
		ss << " {" + std::to_string(m_state.Alteration()) + "}";
		ss << " <line:" << line << ",col:" << col << "> ";
		ss << "'" << ReturnType()->TypeName() << "' ";
		ss << (*m_valueObj);

		return ss.str();
	}

private:
	POLY_IMPL();
};

class CharacterLiteral : public LiteralImpl<char, CharacterLiteral>
{
	NODE_ID(AST::NodeID::CHARACTER_LITERAL_ID);

public:
	template<typename _Ty>
	CharacterLiteral(_Ty&& value)
		: LiteralImpl{ std::forward<_Ty>(value) }
	{
	}
};

class StringLiteral : public LiteralImpl<std::string, StringLiteral>
{
	NODE_ID(AST::NodeID::STRING_LITERAL_ID);

public:
	template<typename _Ty>
	StringLiteral(_Ty&& value)
		: LiteralImpl{ std::forward<_Ty>(value) }
	{
	}
};

class IntegerLiteral : public LiteralImpl<int, IntegerLiteral>
{
	NODE_ID(AST::NodeID::INTEGER_LITERAL_ID);

public:
	template<typename _Ty>
	IntegerLiteral(_Ty&& value)
		: LiteralImpl{ std::forward<_Ty>(value) }
	{
	}
};

class FloatingLiteral : public LiteralImpl<double, FloatingLiteral>
{
	NODE_ID(AST::NodeID::FLOAT_LITERAL_ID);

public:
	template<typename _Ty>
	FloatingLiteral(_Ty&& value)
		: LiteralImpl{ std::forward<_Ty>(value) }
	{
	}
};

//
// Declaration nodes
//

class Decl
	: public Returnable
	, public ASTNode
	, public CoilCl::AST::RefCount
{
	NODE_ID(AST::NodeID::DECL_ID);

protected:
	std::string m_identifier;

public:
	Decl() = default; //TODO: temp, remove afterwards
	//virtual ~Decl() = 0;//TODO

	//TODO: temp, remove afterwards
	Decl(const std::string& name)
		: m_identifier{ name }
	{
	}

	Decl(const std::string& name, const std::shared_ptr<Typedef::TypedefBase>& specifier)
		: m_identifier{ name }
		, Returnable{ AST::TypeFacade{ specifier } }
	{
	}

	virtual void Serialize(Serializable::Interface& pack)
	{
		pack << nodeId;
		pack << m_identifier;
		ASTNode::Serialize(pack);
	}

	virtual void Deserialize(Serializable::Interface& pack)
	{
		AST::NodeID _nodeId;

		pack >> _nodeId;
		AssertNode(_nodeId, nodeId);

		pack >> m_identifier;
		ASTNode::Deserialize(pack);
	}

	auto Identifier() const { return m_identifier; }
};

class VarDecl
	: public Decl
	, public SelfReference<VarDecl>
{
	NODE_ID(AST::NodeID::VAR_DECL_ID);
	std::shared_ptr<ASTNode> m_body;

public:
	VarDecl(const std::string& name, std::shared_ptr<Typedef::TypedefBase> type, std::shared_ptr<ASTNode> node = nullptr)
		: Decl{ name, type }
		, m_body{ node }
	{
		ASTNode::AppendChild(node);
	}

	void Emplace(size_t idx, const std::shared_ptr<ASTNode>&& node) override
	{
		BUMP_STATE();

		ASTNode::RemoveChild(idx);
		ASTNode::AppendChild(node);
		m_body = std::move(node);

		ASTNode::UpdateDelegate();
	}

	virtual const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(VarDecl).name()) };
		_node += " {" + std::to_string(m_state.Alteration()) + "}";
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";

		if (RefCount::IsUsed()) {
			_node += "used ";
		}

		_node += m_identifier;
		_node += " '" + ReturnType().TypeName() + "' ";
		_node += ReturnType()->StorageClassName();

		return _node;
	}

private:
	POLY_IMPL();
};

class ParamDecl
	: public Decl
	, public SelfReference<ParamDecl>
{
	NODE_ID(AST::NodeID::PARAM_DECL_ID);

public:
	ParamDecl(const std::string& name, std::shared_ptr<Typedef::TypedefBase> type)
		: Decl{ name, type }
	{
	}

	ParamDecl(const std::shared_ptr<Typedef::TypedefBase>& type)
		: Decl{ "", type }
	{
	}

	virtual const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(ParamDecl).name()) };
		_node += " {" + std::to_string(m_state.Alteration()) + "}";
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";

		if (m_identifier.empty()) {
			_node += "abstract";
		}
		else {
			_node += m_identifier;
		}

		_node += " '" + ReturnType().TypeName() + "' ";
		_node += ReturnType()->StorageClassName();

		return _node;
	}

private:
	POLY_IMPL();
};

class VariadicDecl
	: public Decl
	, public SelfReference<VariadicDecl>
{
	NODE_ID(AST::NodeID::VARIADIC_DECL_ID);

public:
	VariadicDecl()
		: Decl{ "", std::dynamic_pointer_cast<Typedef::TypedefBase>(Util::MakeVariadicType()) }
	{
	}

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
	TypedefDecl(const std::string& name, std::shared_ptr<Typedef::TypedefBase> type)
		: Decl{ name, type }
	{
	}

	virtual const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(TypedefDecl).name()) };
		_node += " {" + std::to_string(m_state.Alteration()) + "}";
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";
		_node += m_identifier;
		_node += " '" + ReturnType().TypeName() + "' ";
		_node += ReturnType()->StorageClassName();

		return _node;
	}

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
	FieldDecl(const std::string& name, std::shared_ptr<Typedef::TypedefBase> type)
		: Decl{ name, type }
	{
	}

	void SetBitField(const std::shared_ptr<IntegerLiteral>& node)
	{
		ASTNode::AppendChild(NODE_UPCAST(node));
		m_bits = node;

		ASTNode::UpdateDelegate();
	}

	virtual void Serialize(Serializable::Interface& pack)
	{
		pack << nodeId;
		ASTNode::Serialize(pack);
	}

	virtual void Deserialize(Serializable::Interface& pack)
	{
		AST::NodeID _nodeId;
		pack >> _nodeId;
		AssertNode(_nodeId, nodeId);

		Decl::Deserialize(pack);
	}

	virtual const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(FieldDecl).name()) };
		_node += " {" + std::to_string(m_state.Alteration()) + "}";
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";
		_node += m_identifier;
		_node += " '" + ReturnType().TypeName() + "' ";
		_node += ReturnType()->StorageClassName();

		return _node;
	}

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
	RecordDecl(const std::string& name)
		: Decl{ name }
	{
	}

	RecordDecl(RecordType type)
		: Decl{ }
		, m_type{ type }
	{
	}

	auto IsAnonymous() const
	{
		return m_identifier.empty();
	}

	void SetName(const std::string& name)
	{
		m_identifier = name;
	}

	void AddField(std::shared_ptr<FieldDecl>& node)
	{
		ASTNode::AppendChild(NODE_UPCAST(node));
		m_fields.push_back(node);

		ASTNode::UpdateDelegate();
	}

	virtual void Serialize(Serializable::Interface& pack)
	{
		pack << nodeId;
		pack << m_type;
		ASTNode::Serialize(pack);
	}

	virtual void Deserialize(Serializable::Interface& pack)
	{
		AST::NodeID _nodeId;
		pack >> _nodeId;
		AssertNode(_nodeId, nodeId);

		int type;
		pack >> type;
		m_type = static_cast<RecordType>(type);

		Decl::Deserialize(pack);
	}

	virtual const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(RecordDecl).name()) };
		_node += " {" + std::to_string(m_state.Alteration()) + "}";
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";

		_node += m_type == RecordType::STRUCT ? "struct " : "union ";
		_node += IsAnonymous() ? "anonymous" : m_identifier;

		return _node;
	}

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
	EnumConstantDecl(const std::string& name)
		: Decl{ name }
	{
	}

	void SetAssignment(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		m_body = node;

		ASTNode::UpdateDelegate();
	}

	const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(EnumConstantDecl).name()) };
		_node += " {" + std::to_string(m_state.Alteration()) + "}";
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";
		_node += m_identifier;

		if (HasReturnType()) {
			_node += " '" + ReturnType().TypeName() + "' ";
			_node += ReturnType()->StorageClassName();
		}

		return _node;
	}

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
	EnumDecl()
		: Decl{} //TOOD: nope! At least give a name
	{
	}

	auto IsAnonymous() const
	{
		return m_identifier.empty();
	}

	void SetName(const std::string& name)
	{
		m_identifier = name;
	}

	void AddConstant(std::shared_ptr<EnumConstantDecl>& node)
	{
		ASTNode::AppendChild(NODE_UPCAST(node));
		m_constants.push_back(node);

		ASTNode::UpdateDelegate();
	}

	const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(EnumDecl).name()) };
		_node += " {" + std::to_string(m_state.Alteration()) + "}";
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";

		_node += IsAnonymous() ? "anonymous" : m_identifier;

		return _node;
	}

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
	size_t m_useCount = 0;

public:
	explicit FunctionDecl(const std::string& name, std::shared_ptr<CompoundStmt>& node)
		: Decl{ name }
		, m_body{ node }
		, m_isPrototype{ false }
	{
		ASTNode::AppendChild(NODE_UPCAST(node));
	}

	explicit FunctionDecl(const std::string& name, std::shared_ptr<Typedef::TypedefBase> type)
		: Decl{ name, type }
	{
	}

	// If function declaration has a body, its not a prototype
	void SetCompound(const std::shared_ptr<CompoundStmt>& node)
	{
		assert(!m_body);

		ASTNode::AppendChild(NODE_UPCAST(node));
		m_body = node;
		m_isPrototype = false;

		ASTNode::UpdateDelegate();
	}

	void SetSignature(std::vector<AST::TypeFacade>&& signature)
	{
		assert(!signature.empty());

		m_signature = std::move(signature);
	}

	void SetParameterStatement(const std::shared_ptr<ParamStmt>& node)
	{
		assert(!m_params);

		ASTNode::AppendChild(NODE_UPCAST(node));
		m_params = node;

		ASTNode::UpdateDelegate();
	}

	auto HasSignature() const { return !m_signature.empty(); }
	auto& Signature() const { return m_signature; }
	auto& ParameterStatement() const { return m_params; }
	auto IsPrototypeDefinition() const { return m_isPrototype; }
	auto HasPrototypeDefinition() const { return !m_protoRef.expired(); }
	auto PrototypeDefinition() const { return m_protoRef.lock(); }

	// Bind function body to prototype definition
	void BindPrototype(const std::shared_ptr<FunctionDecl>& node)
	{
		assert(!m_isPrototype);
		assert(m_protoRef.expired());

		m_protoRef = node;
	}

	const std::string NodeName() const
	{
		std::stringstream ss;
		ss << RemoveClassFromName(typeid(FunctionDecl).name());
		ss << " {" + std::to_string(m_state.Alteration()) + "}";
		ss << " <line:" << line << ",col:" << col << "> ";

		if (IsPrototypeDefinition()) {
			ss << "proto ";
		}
		else if (HasPrototypeDefinition()) {
			ss << "linked ";
		}

		if (RefCount::IsUsed()) {
			ss << "used ";
		}

		ss << m_identifier;

		ss << " '" << ReturnType().TypeName() + " (";
		for (auto it = m_signature.begin(); it != m_signature.end(); ++it) {
			ss << it->TypeName();
			if (m_signature.size() > 1 && it != m_signature.end() - 1) {
				ss << ", ";
			}
		}
		ss << ")' ";

		ss << ReturnType()->StorageClassName() << " ";

		if (ReturnType()->IsInline()) {
			ss << "inline";
		}

		return ss.str();
	}

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
	void AppendChild(const std::shared_ptr<ASTNode>& node) final
	{
		ASTNode::AppendChild(node);
		m_children.push_back(node);

		ASTNode::UpdateDelegate();
	}

	template<typename... _Args>
	static std::shared_ptr<TranslationUnitDecl> Make(_Args&&... args)
	{
		auto ptr = std::shared_ptr<TranslationUnitDecl>{ new TranslationUnitDecl{ std::forward<_Args>(args)... } };
		ptr->UpdateDelegate();
		return ptr;
	}

	virtual void Serialize(Serializable::Interface& pack)
	{
		pack << nodeId;
		Decl::Serialize(pack);
	}

	virtual void Deserialize(Serializable::Interface& pack)
	{
		AST::NodeID _nodeId;
		pack >> _nodeId;
		AssertNode(_nodeId, nodeId);

		Decl::Deserialize(pack);
	}

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
	NODE_ID(AST::NodeID::EXPR_ID);

public:
	virtual ~Expr() = 0;
};

class ResolveRefExpr
	: public Expr
	, public SelfReference<ResolveRefExpr>
{
	NODE_ID(AST::NodeID::RESOLVE_REF_EXPR_ID);
	std::string m_identifier;

public:
	ResolveRefExpr(const std::string& identifier)
		: m_identifier{ identifier }
	{
	}

	ResolveRefExpr() = default;

	//TODO: friend
	auto Identifier() const
	{
		return m_identifier;
	}

protected:
	const std::string NodeName() const
	{
		return std::string{ RemoveClassFromName(typeid(ResolveRefExpr).name()) } +" {" + std::to_string(m_state.Alteration()) + "}" + " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> '" + m_identifier + "'";
	}

private:
	POLY_IMPL();
};

class DeclRefExpr
	: public ResolveRefExpr
{
	NODE_ID(AST::NodeID::DECL_REF_EXPR_ID);
	std::weak_ptr<Decl> m_ref;

public:
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

	void Resolve(const std::shared_ptr<ASTNode>& ref)
	{
		m_ref = std::dynamic_pointer_cast<Decl>(ref);
	}

	bool HasReturnType() const override
	{
		return IsResolved();
	}

	AST::TypeFacade ReturnType() const override
	{
		if (IsResolved()) {
			return Reference()->ReturnType();
		}

		return Returnable::ReturnType();
	}

	const std::string NodeName() const
	{
		if (IsResolved()) {
			std::string _node{ RemoveClassFromName(typeid(DeclRefExpr).name()) };
			_node += " {" + std::to_string(m_state.Alteration()) + "}";
			_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";
			_node += "linked '" + m_ref.lock()->Identifier() + "' ";

			if (Reference()->ReturnType().HasValue()) {
				_node += "'" + Reference()->ReturnType().TypeName() + "' ";
				_node += Reference()->ReturnType()->StorageClassName();
			}

			return _node;
		}

		return ResolveRefExpr::NodeName();
	}

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
	CallExpr(std::shared_ptr<DeclRefExpr>& func, std::shared_ptr<ArgumentStmt> args = nullptr)
		: Expr{}
	{
		ASTNode::AppendChild(NODE_UPCAST(func));
		m_funcRef = func;

		if (args != nullptr) {
			ASTNode::AppendChild(NODE_UPCAST(args));
			m_args = args;
		}
	}

	const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(CallExpr).name()) };
		_node += " {" + std::to_string(m_state.Alteration()) + "}";
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";

		if (ReturnType().HasValue()) {
			_node += "'" + ReturnType().TypeName() + "' ";
			_node += ReturnType()->StorageClassName();
		}

		return _node;
	}

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
	BuiltinExpr(std::shared_ptr<DeclRefExpr>& func, std::shared_ptr<DeclRefExpr> expr = nullptr, std::shared_ptr<ArgumentStmt> args = nullptr)
		: CallExpr{ func, args }
		, m_expr{ expr }
	{
		if (expr != nullptr) {
			ASTNode::AppendChild(NODE_UPCAST(expr));
		}
	}

	void SetExpression(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		m_expr = node;

		ASTNode::UpdateDelegate();
	}

	//TODO: move?
	void SetTypename(std::shared_ptr<Typedef::TypedefBase>& type)
	{
		m_typenameType = type;
	}

	///TODO: friends
	auto Expression() const { return m_expr; }
	auto TypeName() const { return m_typenameType; }

	const std::string NodeName() const final
	{
		return std::string{ RemoveClassFromName(typeid(BuiltinExpr).name()) } +" {" + std::to_string(m_state.Alteration()) + "}" + " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";
	}

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
	CastExpr(std::shared_ptr<ASTNode>& node)
		: Expr{}
	{
		ASTNode::AppendChild(node);
		rtype = node;
	}

	const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(CastExpr).name()) };
		_node += " {" + std::to_string(m_state.Alteration()) + "}";
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";

		return _node;
	}

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
	ImplicitConvertionExpr(std::shared_ptr<ASTNode>& node, Conv::Cast::Tag convOp)
		: m_body{ node }
		, m_convOp{ convOp }
	{
		ASTNode::AppendChild(node);
	}

	virtual const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(ImplicitConvertionExpr).name()) };
		_node += " {" + std::to_string(m_state.Alteration()) + "}";
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";

		if (ReturnType().HasValue()) {
			_node += "'" + ReturnType().TypeName() + "' ";
			_node += ReturnType()->StorageClassName();
		}

		_node += "[" + Conv::Cast::PrintTag(m_convOp) + "]";

		return _node;
	}

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
	ParenExpr(std::shared_ptr<ASTNode>& node)
		: m_body{ node }
	{
		ASTNode::AppendChild(node);
	}

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
	void AddListItem(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		m_children.push_back(node);

		ASTNode::UpdateDelegate();
	}

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
	CompoundLiteralExpr(std::shared_ptr<InitListExpr>& node)
		: m_body{ node }
	{
		ASTNode::AppendChild(NODE_UPCAST(node));
	}

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
	ArraySubscriptExpr(std::shared_ptr<DeclRefExpr>& ref, std::shared_ptr<ASTNode>& expr)
		: m_identifier{ ref }
		, m_offset{ expr }
	{
		ASTNode::AppendChild(NODE_UPCAST(ref));
		ASTNode::AppendChild(expr);
	}

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
	MemberExpr(MemberType type, const std::string& name, std::shared_ptr<DeclRefExpr>& node)
		: m_memberType{ type }
		, m_name{ name }
		, m_record{ node }
	{
		ASTNode::AppendChild(NODE_UPCAST(node));
	}

	const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(MemberExpr).name()) };
		_node += " {" + std::to_string(m_state.Alteration()) + "}";
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";
		_node += m_memberType == MemberType::REFERENCE ? "." : "->";
		_node += m_name;

		return _node;
	}

private:
	POLY_IMPL();
};

//
// Statement nodes
//

class Stmt : public ASTNode
{
	NODE_ID(AST::NodeID::STMT_ID);

public:
	virtual ~Stmt() = 0;
};

class ContinueStmt
	: public Stmt
	, public SelfReference<ContinueStmt>
{
	NODE_ID(AST::NodeID::CONTINUE_STMT_ID);

public:
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
	void SetReturnNode(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		m_returnExpr = node;

		ASTNode::UpdateDelegate();
	}

	auto HasExpression() const { return m_returnExpr != nullptr; }
	auto& Expression() const { return m_returnExpr; }

	void Emplace(size_t idx, const std::shared_ptr<ASTNode>&& node) override
	{
		BUMP_STATE();

		ASTNode::RemoveChild(idx);
		ASTNode::AppendChild(node);
		m_returnExpr = std::move(node);

		ASTNode::UpdateDelegate();
	}

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
	IfStmt(std::shared_ptr<ASTNode>& eval, std::shared_ptr<ASTNode> truth = nullptr, std::shared_ptr<ASTNode> alt = nullptr)
		: m_evalNode{ eval }
	{
		ASTNode::AppendChild(eval);

		if (truth) {
			ASTNode::AppendChild(truth);
			m_truthStmt = truth;
		}

		if (alt) {
			ASTNode::AppendChild(alt);
			m_altStmt = alt;
		}
	}

	void SetTruthCompound(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		m_truthStmt = node;

		ASTNode::UpdateDelegate();
	}

	void SetAltCompound(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		m_altStmt = node;

		ASTNode::UpdateDelegate();
	}

	virtual const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(IfStmt).name()) };
		_node += " {" + std::to_string(m_state.Alteration()) + "}";
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";

		if (m_truthStmt) {
			_node += "notruth ";
		}

		if (m_altStmt == nullptr) {
			_node += "noalt ";
		}

		return _node;
	}

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
	SwitchStmt(std::shared_ptr<ASTNode>& eval, std::shared_ptr<ASTNode> body = nullptr)
		: evalNode{ eval }
	{
		ASTNode::AppendChild(eval);

		if (body) {
			ASTNode::AppendChild(body);
			m_body = body;
		}
	}

	void SetBody(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		m_body = node;

		ASTNode::UpdateDelegate();
	}

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
	WhileStmt(std::shared_ptr<ASTNode>& eval, std::shared_ptr<ASTNode> body = nullptr)
		: evalNode{ eval }
	{
		ASTNode::AppendChild(eval);

		if (body) {
			ASTNode::AppendChild(body);
			m_body = body;
		}
	}

	void SetBody(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		m_body = node;

		ASTNode::UpdateDelegate();
	}

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
	DoStmt(std::shared_ptr<ASTNode>& body, std::shared_ptr<ASTNode> eval = nullptr)
		: m_body{ body }
	{
		ASTNode::AppendChild(body);

		if (eval) {
			ASTNode::AppendChild(eval);
			m_body = eval;
		}
	}

	void SetEval(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		evalNode = node;

		ASTNode::UpdateDelegate();
	}

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
	ForStmt(std::shared_ptr<ASTNode>& node1, std::shared_ptr<ASTNode>& node2, std::shared_ptr<ASTNode>& node3)
		: m_node1{ node1 }
		, m_node2{ node2 }
		, m_node3{ node3 }
	{
		ASTNode::AppendChild(node1);
		ASTNode::AppendChild(node2);
		ASTNode::AppendChild(node3);
	}

	void SetBody(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		m_body = node;

		ASTNode::UpdateDelegate();
	}

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
	BreakStmt()
	{
	}

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
	DefaultStmt(const std::shared_ptr<ASTNode>& body)
		: m_body{ body }
	{
		ASTNode::AppendChild(body);
	}

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
	CaseStmt(std::shared_ptr<ASTNode>& name, std::shared_ptr<ASTNode>& body)
		: m_name{ name }
		, m_body{ body }
	{
		ASTNode::AppendChild(name);
		ASTNode::AppendChild(body);
	}

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
	void AddDeclaration(const std::shared_ptr<VarDecl>& node)
	{
		ASTNode::AppendChild(NODE_UPCAST(node));
		m_var.push_back(node);

		ASTNode::UpdateDelegate();
	}

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
	void AppendArgument(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		m_arg.push_back(node);

		ASTNode::UpdateDelegate();
	}

	void Emplace(size_t idx, const std::shared_ptr<ASTNode>&& node) override final
	{
		BUMP_STATE();

		ASTNode::RemoveChild(idx);
		ASTNode::AppendChild(node);
		m_arg[idx] = std::move(node);

		ASTNode::UpdateDelegate();
	}

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
	void AppendParamter(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		m_param.push_back(node);

		ASTNode::UpdateDelegate();
	}

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
	LabelStmt(const std::string& name, std::shared_ptr<ASTNode>& node)
		: m_name{ name }
		, m_body{ node }
	{
		ASTNode::AppendChild(node);
	}

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
	GotoStmt(const std::string& name)
		: m_labelName{ name }
	{
	}

	virtual const std::string NodeName() const
	{
		return std::string{ RemoveClassFromName(typeid(GotoStmt).name()) } +" {" + std::to_string(m_state.Alteration()) + "}" + " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> '" + m_labelName + "'";
	}

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
	void AppendChild(const std::shared_ptr<ASTNode>& node) final
	{
		ASTNode::AppendChild(node);
		m_children.push_back(node);

		ASTNode::UpdateDelegate();
	}

	PRINT_NODE(CompoundStmt);

private:
	POLY_IMPL();
};
