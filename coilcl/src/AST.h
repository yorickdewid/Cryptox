// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Typedef.h"
#include "Valuedef.h"

#include <list>
#include <vector>
#include <memory>
#include <sstream>

#define PRINT_NODE(n) \
	virtual const std::string NodeName() const { \
		return std::string{ RemoveClassFromName(typeid(n).name()) } + " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + ">"; \
	}

#define NODE_UPCAST(c) \
	std::dynamic_pointer_cast<ASTNode>(c)

using namespace CoilCl;

template<typename _Ty>
std::string RemoveClassFromName(_Ty *_name)
{
	constexpr const char stripClassStr[] = "class";
	std::string f{ _name };
	if (size_t pos = f.find_last_of(stripClassStr) != std::string::npos) {
		return f.substr(pos + sizeof(stripClassStr) - 1);
	}
	return f;
}

class DeclRefExpr;
class CompoundStmt;
class ArgumentStmt;
class ParamStmt;

class ASTNode
{
protected:
	int line = -1;
	int col = -1;

public:
	ASTNode() = default;
	ASTNode(int _line, int _col)
		: line{ _line }
		, col{ _col }
	{
	}

	inline size_t ChildrenCount() const
	{
		return children.size();
	}

	void SetLocation(int _line, int _col)
	{
		line = _line;
		col = _col;
	}

	void SetLocation(std::pair<int, int>& loc)
	{
		line = loc.first;
		col = loc.second;
	}

	virtual const std::string NodeName() const = 0;

	void Print(int level = 0, bool last = false);

	void SetParent(std::shared_ptr<ASTNode>& node)
	{
		parent = node;
	}

protected:
	virtual void AppendChild(std::shared_ptr<ASTNode>& node)
	{
		children.push_back(node);
	}

protected:
	std::list<std::weak_ptr<ASTNode>> children;
	std::weak_ptr<ASTNode> parent;//TODO
};

//
// Operator nodes
//

class Operator : public ASTNode
{
protected:
	// return type

public:
	PRINT_NODE(Operator);
};

class BinaryOperator : public Operator
{
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
	BinaryOperator(BinOperand operand, std::shared_ptr<ASTNode>& leftSide)
		: m_operand{ operand }
		, m_lhs{ leftSide }
	{
		//node->SetParent(NODE_UPCAST(GetSharedSelf()));

		ASTNode::AppendChild(leftSide);
	}

	void SetRightSide(std::shared_ptr<ASTNode>& node)
	{
		//node->SetParent(NODE_UPCAST(GetSharedSelf()));

		ASTNode::AppendChild(node);
		m_rhs = node;
	}

	const std::string NodeName() const
	{
		return std::string{ RemoveClassFromName(typeid(BinaryOperator).name()) } +" <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> 'return type' '" + BinOperandStr(m_operand) + "'";
	}
};

class ConditionalOperator : public Operator
{
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
	}

	void SetAltCompound(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		altStmt = node;
	}

	virtual const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(ConditionalOperator).name()) };
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";

		return _node;
	}
};

namespace CoilCl
{
namespace AST
{

class UnaryOperator : public Operator
{
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
	UnaryOperator(UnaryOperand operand, OperandSide side, std::shared_ptr<ASTNode>& node)
		: m_operand{ operand }
		, m_side{ side }
	{
		ASTNode::AppendChild(node);
		m_body = node;
	}

	const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(UnaryOperator).name()) };
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> 'return type' ";

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
};

} // namespace AST
} // namespace CoilCl

class CompoundAssignOperator : public Operator
{
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
	CompoundAssignOperator(CompoundAssignOperand operand, std::shared_ptr<DeclRefExpr>& node)
		: m_operand{ operand }
	{
		ASTNode::AppendChild(NODE_UPCAST(node));
		m_identifier = node;
	}

	void SetRightSide(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		m_body = node;
	}

	const std::string NodeName() const
	{
		return std::string{ RemoveClassFromName(typeid(CompoundAssignOperator).name()) } +" <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> 'return type' '" + CompoundAssignOperandStr(m_operand) + "'";
	}
};

//
// Literal nodes
//

template<typename _NativTy, class _DrivTy>
class Literal : public ASTNode
{
protected:
	std::shared_ptr<CoilCl::Valuedef::ValueObject<_NativTy>> m_valueObj;

public:
	// Default to void type with no data
	Literal()
		: m_valueObj{ new CoilCl::Valuedef::ValueObject<void>{Value::TypeSpecifier::T_VOID} }
	{
	}

	// Move data object from lexer into literal
	Literal(std::shared_ptr<CoilCl::Valuedef::ValueObject<_NativTy>>&& object)
		: m_valueObj{ std::move(object) }
	{
	}

	const std::string NodeName() const
	{
		std::stringstream ss;
		ss << RemoveClassFromName(typeid(_DrivTy).name());
		ss << " <line:" << line << ",col:" << col << "> ";
		ss << "'" << m_valueObj->DataType()->TypeName() << "' ";
		ss << (*m_valueObj);

		return ss.str();
	}
};

class CharacterLiteral : public Literal<char, CharacterLiteral>
{
public:
	template<typename _Ty>
	CharacterLiteral(_Ty&& value)
		: Literal{ std::forward<_Ty>(value) }
	{
	}
};

class StringLiteral : public Literal<std::string, StringLiteral>
{
public:
	template<typename _Ty>
	StringLiteral(_Ty&& value)
		: Literal{ std::forward<_Ty>(value) }
	{
	}
};

class IntegerLiteral : public Literal<int, IntegerLiteral>
{
public:
	template<typename _Ty>
	IntegerLiteral(_Ty&& value)
		: Literal{ std::forward<_Ty>(value) }
	{
	}
};

class FloatingLiteral : public Literal<double, FloatingLiteral>
{
public:
	template<typename _Ty>
	FloatingLiteral(_Ty&& value)
		: Literal{ std::forward<_Ty>(value) }
	{
	}
};

//
// Declaration nodes
//

class Decl : public ASTNode
{
protected:
	std::string m_identifier;
	std::shared_ptr<Typedef::TypedefBase> m_returnType;

public:
	Decl() = default;

	//TODO: temp, remove afterwards
	Decl(const std::string& name)
		: m_identifier{ name }
	{
	}

	template<typename _TySpec>
	Decl(const std::string& name, _TySpec specifier)
		: m_identifier{ name }
		, m_returnType{ specifier }
	{
	}

	auto Identifier() const { return m_identifier; }

	PRINT_NODE(Decl);
};

class VarDecl : public Decl
{
	std::shared_ptr<ASTNode> m_body;

public:
	VarDecl(const std::string& name, std::shared_ptr<Typedef::TypedefBase> type, std::shared_ptr<ASTNode> node = nullptr)
		: Decl{ name, type }
		, m_body{ node }
	{
		ASTNode::AppendChild(node);
	}

	virtual const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(VarDecl).name()) };
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";
		_node += m_identifier;
		_node += " '" + Decl::m_returnType->TypeName() + "' ";
		_node += Decl::m_returnType->StorageClassName();

		return _node;
	}
};

class ParamDecl : public Decl
{
public:
	ParamDecl(const std::string& name)
		: Decl{ name }
	{
	}

	virtual const std::string NodeName() const
	{
		return std::string{ RemoveClassFromName(typeid(ParamDecl).name()) } +" <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> " + m_identifier;
	}
};

class TypedefDecl : public Decl
{
public:
	TypedefDecl(const std::string& name, std::shared_ptr<Typedef::TypedefBase> type)
		: Decl{ name, type }
	{
	}

	virtual const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(TypedefDecl).name()) };
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";
		_node += m_identifier;
		_node += " '" + Decl::m_returnType->TypeName() + "' ";
		_node += Decl::m_returnType->StorageClassName();

		return _node;
	}
};

class FieldDecl : public Decl
{
	std::shared_ptr<IntegerLiteral> m_bits;

public:
	FieldDecl(const std::string& name, std::shared_ptr<Typedef::TypedefBase> type)
		: Decl{ name, type }
	{
	}

	void SetBitField(std::shared_ptr<IntegerLiteral>& node)
	{
		ASTNode::AppendChild(NODE_UPCAST(node));
		m_bits = node;
	}

	virtual const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(FieldDecl).name()) };
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";
		_node += m_identifier;
		_node += " '" + Decl::m_returnType->TypeName() + "' ";
		_node += Decl::m_returnType->StorageClassName();

		return _node;
	}
};

class RecordDecl : public Decl
{
	std::vector<std::shared_ptr<FieldDecl>> m_fields;

public:
	enum RecordType
	{
		STRUCT,
		UNION,
	} m_type = RecordType::STRUCT;

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
	}

	virtual const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(RecordDecl).name()) };
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";

		_node += m_type == RecordType::STRUCT ? "struct " : "union ";
		_node += IsAnonymous() ? "anonymous" : m_identifier;

		return _node;
	}
};

class EnumConstantDecl : public Decl
{
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
	}

	virtual const std::string NodeName() const
	{
		return std::string{ RemoveClassFromName(typeid(EnumConstantDecl).name()) } +" <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> " + m_identifier;
	}
};

class EnumDecl : public Decl
{
	std::vector<std::shared_ptr<EnumConstantDecl>> m_constants;

public:
	EnumDecl()
		: Decl{}
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
	}

	virtual const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(EnumDecl).name()) };
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";

		_node += IsAnonymous() ? "anonymous" : m_identifier;

		return _node;
	}
};

class FunctionDecl : public Decl
{
	std::shared_ptr<ParamStmt> m_params;
	std::shared_ptr<CompoundStmt> m_body;
	std::weak_ptr<FunctionDecl> m_protoRef;

	bool m_isPrototype = true;
	size_t m_useCount = 0;

private:
	auto IsUsed() const { return m_useCount > 0; }

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
	void SetCompound(std::shared_ptr<CompoundStmt>& node)
	{
		assert(!m_body);

		ASTNode::AppendChild(NODE_UPCAST(node));
		m_body = node;
		m_isPrototype = false;
	}

	void SetParameterStatement(std::shared_ptr<ParamStmt>& node)
	{
		assert(!m_params);

		ASTNode::AppendChild(NODE_UPCAST(node));
		m_params = node;
	}

	auto IsPrototypeDefinition() const { return m_isPrototype; }
	auto HasPrototypeDefinition() const { return !m_protoRef.expired(); }

	// Bind function body to prototype definition
	void BindPrototype(std::shared_ptr<FunctionDecl>& node)
	{
		assert(!m_isPrototype);
		assert(m_protoRef.expired());

		m_protoRef = node;
	}

	void RegisterCaller()
	{
		m_useCount++;
	}

	const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(FunctionDecl).name()) };
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";

		if (IsPrototypeDefinition()) {
			_node += "proto ";
		}
		else if (HasPrototypeDefinition()) {
			_node += "linked ";

			if (m_protoRef.lock()->IsUsed()) {
				_node += "used ";
			}
		}

		if (IsUsed()) {
			_node += "used ";
		}

		_node += m_identifier;
		_node += " '" + Decl::m_returnType->TypeName() + "' ";
		_node += Decl::m_returnType->StorageClassName();

		return _node;
	}

	/*FunctionDecl(std::unique_ptr<Value>& value)
	{
		assert(value->DataType() == Value::TypeSpecifier::T_CHAR);

		if (value->IsArray()) {
			identifier = value->As<std::string>();
		}
		else {
			identifier.push_back(value->As<char>());
		}
	}

	void ReturnType(std::unique_ptr<ASTNode> rtnVal)
	{
		returnType = std::move(rtnVal);
	}*/
};

class TranslationUnitDecl
	: public Decl
	, public std::enable_shared_from_this<TranslationUnitDecl>
{
	std::list<std::shared_ptr<ASTNode>> m_children;

	std::shared_ptr<TranslationUnitDecl> GetSharedSelf()
	{
		return shared_from_this();
	}

public:
	TranslationUnitDecl(const std::string& sourceName)
		: Decl{ sourceName }
	{
	}

	void AppendChild(std::shared_ptr<ASTNode>& node) final
	{
		node->SetParent(NODE_UPCAST(GetSharedSelf()));

		ASTNode::AppendChild(node);
		m_children.push_back(node);
	}

	PRINT_NODE(TranslationUnitDecl);
};

//
// Expression nodes
//

class Expr : public ASTNode
{
protected:
	// return type

public:
	PRINT_NODE(Expr);
};

class ResolveRefExpr : public Expr
{
	std::string m_identifier;

public:
	ResolveRefExpr(const std::string& identifier)
		: m_identifier{ identifier }
	{
	}

	ResolveRefExpr() = default;

protected:
	const std::string NodeName() const
	{
		return std::string{ RemoveClassFromName(typeid(ResolveRefExpr).name()) } +" <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> '" + m_identifier + "'";
	}
};

class DeclRefExpr : public ResolveRefExpr
{
	std::weak_ptr<Decl> m_ref;

private:
	auto IsResolved() const { return !m_ref.expired(); }

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

	const std::string NodeName() const
	{
		if (IsResolved()) {
			return std::string{ RemoveClassFromName(typeid(DeclRefExpr).name()) } +" <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> linked '" + m_ref.lock()->Identifier() + "'";
		}
		else {
			return ResolveRefExpr::NodeName();
		}
	}
};

class CallExpr : public Expr
{
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
		return std::string{ RemoveClassFromName(typeid(CallExpr).name()) } +" <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> 'return type'";
	}
};

class BuiltinExpr : public CallExpr
{
	std::shared_ptr<DeclRefExpr> m_funcRef;
	std::shared_ptr<ArgumentStmt> m_args;
	std::shared_ptr<ASTNode> m_rvalue;

public:
	BuiltinExpr(std::shared_ptr<DeclRefExpr>& func, std::shared_ptr<DeclRefExpr> expr = nullptr, std::shared_ptr<ArgumentStmt> args = nullptr)
		: CallExpr{ func, args }
		, m_rvalue{ expr }
	{
		if (expr != nullptr) {
			ASTNode::AppendChild(NODE_UPCAST(expr));
		}
	}

	void SetExpression(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		m_rvalue = node;
	}

	const std::string NodeName() const
	{
		return std::string{ RemoveClassFromName(typeid(BuiltinExpr).name()) } +" <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> 'return type'";
	}
};

class CastExpr : public Expr
{
	std::shared_ptr<ASTNode> rtype;
	bool m_implicit = true;

public:
	CastExpr(std::shared_ptr<ASTNode>& node, bool implicit = false)
		: Expr{}
		, m_implicit{ implicit }
	{
		ASTNode::AppendChild(node);
		rtype = node;
	}

	const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(CastExpr).name()) };
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";

		if (m_implicit) {
			_node += "implicit ";
		}

		return _node;
	}
};

class ParenExpr : public Expr
{
	std::shared_ptr<ASTNode> m_body;

public:
	ParenExpr(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		m_body = node;
	}

	PRINT_NODE(ParenExpr);
};

class InitListExpr : public Expr
{
	std::vector<std::shared_ptr<ASTNode>> m_children;

public:
	void AddListItem(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		m_children.push_back(node);
	}

	PRINT_NODE(InitListExpr);
};

class CompoundLiteralExpr : public Expr
{
	std::shared_ptr<InitListExpr> m_body;

public:
	CompoundLiteralExpr(std::shared_ptr<InitListExpr>& node)
		: m_body{ node }
	{
		ASTNode::AppendChild(NODE_UPCAST(node));
	}

	PRINT_NODE(CompoundLiteralExpr);
};

class ArraySubscriptExpr : public Expr
{
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
};

class MemberExpr : public Expr
{
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
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";
		_node += m_memberType == MemberType::REFERENCE ? "." : "->";
		_node += m_name;

		return _node;
	}
};

//
// Statement nodes
//

class Stmt : public ASTNode
{
public:
	PRINT_NODE(Stmt);
};

//TODO: remove ?
class NullStmt : public Stmt
{
public:
	PRINT_NODE(NullStmt);
};

class ContinueStmt : public Stmt
{
public:
	PRINT_NODE(ContinueStmt);
};

class ReturnStmt : public Stmt
{
	std::shared_ptr<ASTNode> m_returnNode;

public:
	void SetReturnNode(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		m_returnNode = node;
	}

	PRINT_NODE(ReturnStmt);
};

class IfStmt : public Stmt
{
	std::shared_ptr<ASTNode> evalNode;
	std::shared_ptr<ASTNode> truthStmt;
	std::shared_ptr<ASTNode> altStmt;

public:
	IfStmt(std::shared_ptr<ASTNode>& eval, std::shared_ptr<ASTNode> truth = nullptr, std::shared_ptr<ASTNode> alt = nullptr)
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
	}

	void SetAltCompound(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		altStmt = node;
	}

	virtual const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(IfStmt).name()) };
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";

		if (truthStmt == nullptr) {
			_node += "notruth ";
		}

		if (altStmt == nullptr) {
			_node += "noalt ";
		}

		return _node;
	}
};

class SwitchStmt : public Stmt
{
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
	}

	PRINT_NODE(SwitchStmt);
};

class WhileStmt : public Stmt
{
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
	}

	PRINT_NODE(WhileStmt);
};

class DoStmt : public Stmt
{
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
	}

	PRINT_NODE(DoStmt);
};

class ForStmt : public Stmt
{
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
	}

	PRINT_NODE(ForStmt);
};

class BreakStmt : public Stmt
{

public:
	BreakStmt()
	{
	}

	PRINT_NODE(BreakStmt);
};

class DefaultStmt : public Stmt
{
	std::shared_ptr<ASTNode> m_body;

public:
	DefaultStmt(std::shared_ptr<ASTNode>& body)
		: m_body{ body }
	{
		ASTNode::AppendChild(body);
	}

	PRINT_NODE(DefaultStmt);
};

class CaseStmt : public Stmt
{
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
};

class DeclStmt : public Stmt
{
	std::list<std::shared_ptr<VarDecl>> m_var;

public:
	void AddDeclaration(std::shared_ptr<VarDecl>& node)
	{
		ASTNode::AppendChild(NODE_UPCAST(node));
		m_var.push_back(node);
	}

	PRINT_NODE(DeclStmt);
};

class ArgumentStmt : public Stmt
{
	std::vector<std::shared_ptr<ASTNode>> m_arg;

public:
	void AppendArgument(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		m_arg.push_back(node);
	}

	PRINT_NODE(ArgumentStmt);
};

class ParamStmt : public Stmt
{
	std::vector<std::shared_ptr<ASTNode>> m_param;

public:
	void AppendParamter(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		m_param.push_back(node);
	}

	PRINT_NODE(ParamStmt);
};

class LabelStmt : public Stmt
{
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
};

class GotoStmt : public Stmt
{
	std::string m_labelName;

public:
	GotoStmt(const std::string& name)
		: m_labelName{ name }
	{
	}

	virtual const std::string NodeName() const
	{
		return std::string{ RemoveClassFromName(typeid(GotoStmt).name()) } +" <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> '" + m_labelName + "'";
	}
};

class CompoundStmt : public Stmt
{
	std::list<std::shared_ptr<ASTNode>> m_children;

public:
	void AppendChild(std::shared_ptr<ASTNode>& node) final
	{
		ASTNode::AppendChild(node);
		m_children.push_back(node);
	}

	PRINT_NODE(CompoundStmt);
};

#if as
class ValueNode : public ASTNode
{
	std::unique_ptr<Value> value = nullptr;

public:
	ValueNode()
		: value{ new ValueObject<void>(Value::TypeSpecifier::T_VOID) }
	{
	}

	ValueNode(std::unique_ptr<Value>& _value)
	{
		value = std::move(_value);
	}

	/*ValueNode(std::function<Value *(void)> valueDelegate)
	{
		auto x = valueDelegate();
		value = std::unique_ptr<Value>(valueDelegate());
	}*/
};
#endif
