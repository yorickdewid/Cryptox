// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Typedef.h" //TODO: remove ?
#include "Valuedef.h"
#include "TypeFacade.h"

#include <vector>
#include <memory>
#include <sstream>

#define PRINT_NODE(n) \
	virtual const std::string NodeName() const { \
		return std::string{ RemoveClassFromName(typeid(n).name()) } + " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + ">"; \
	}

#define NODE_UPCAST(c) \
	std::dynamic_pointer_cast<ASTNode>(c)

#define POLY_IMPL() \
	std::shared_ptr<ASTNode> PolySelf() { \
		return std::dynamic_pointer_cast<ASTNode>(GetSharedSelf()); \
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

template <typename _Ty>
struct Identity { using type = typename _Ty::value_type; };

template<typename _Ty, typename _Base = ASTNode>
class SelfReference : public std::enable_shared_from_this<_Ty>
{
protected:
	std::shared_ptr<_Base> GetSharedSelf()
	{
		return shared_from_this();
	}
};

class DeclRefExpr;
class CompoundStmt;
class ArgumentStmt;
class ParamStmt;

class ASTNode
{
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

	inline size_t ChildrenCount() const
	{
		return children.size();
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

	virtual const std::string NodeName() const = 0;
	virtual std::shared_ptr<ASTNode> PolySelf() = 0;

	//TODO: replace with for_each ?
	void Print(int level = 0, bool last = 0, std::vector<int> ignore = {});

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

	void UpdateDelegate()
	{
		std::transform(children.begin(), children.end(), children.begin(), [this](Identity<decltype(children)>::type wPtr)
		{
			if (auto ptr = wPtr.lock()) {
				ptr->SetParent(std::move(this->PolySelf()));
			}

			return wPtr;
		});
	}

protected:
	virtual void AppendChild(const std::shared_ptr<ASTNode>& node)
	{
		children.push_back(node);
	}

	void SetParent(const std::shared_ptr<ASTNode>&& node)
	{
		m_parent = node;
	}

protected:
	std::vector<std::weak_ptr<ASTNode>> children;
	std::weak_ptr<ASTNode> m_parent;
};

//
// Operator nodes
//

class Operator : public ASTNode
{
protected:
	std::shared_ptr<Typedef::TypedefBase> m_returnType;
};

class BinaryOperator
	: public Operator
	, public SelfReference<BinaryOperator>
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

	const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(BinaryOperator).name()) };
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";

#if 0
		if (m_returnType) {
			_node + "'return type' ";//TODO
		}
#endif

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
		return std::string{ RemoveClassFromName(typeid(CompoundAssignOperator).name()) } +" <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> '" + CompoundAssignOperandStr(m_operand) + "'";
	}

private:
	POLY_IMPL();
};

//
// Literal nodes
//

template<typename _NativTy, class _DrivTy>
class Literal
	: public ASTNode
	, public SelfReference<_DrivTy>
{
protected:
	std::shared_ptr<CoilCl::Valuedef::ValueObject<_NativTy>> m_valueObj;

public:
	// Default to void type with no data
	Literal()
		: m_valueObj{ CoilCl::Typedef::BuiltinType{ CoilCl::Typedef::BuiltinType::Specifier::VOID } }
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

private:
	POLY_IMPL();
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
	AST::TypeFacade m_returnType;

public:
	Decl() = default; //TODO: temp, remove afterwards
	virtual ~Decl() = 0;

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

	auto& ReturnType() { return m_returnType; }
	auto& ReturnType() const { return m_returnType; }
	auto Identifier() const { return m_identifier; }
};

class VarDecl
	: public Decl
	, public SelfReference<VarDecl>
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
		_node += " '" + Decl::ReturnType().TypeName() + "' ";
		_node += Decl::ReturnType()->StorageClassName();

		return _node;
	}

private:
	POLY_IMPL();
};

class ParamDecl
	: public Decl
	, public SelfReference<ParamDecl>
{
public:
	ParamDecl(const std::string& name, std::shared_ptr<Typedef::TypedefBase> type)
		: Decl{ name, type }
	{
	}

	ParamDecl(std::shared_ptr<Typedef::TypedefBase> type)
		: Decl{ "", type }
	{
	}

	virtual const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(ParamDecl).name()) };
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";

		if (m_identifier.empty()) {
			_node += "abstract";
		}
		else {
			_node += m_identifier;
		}

		_node += " '" + Decl::ReturnType().TypeName() + "' ";
		_node += Decl::ReturnType()->StorageClassName();

		return _node;
	}

private:
	POLY_IMPL();
};

class VariadicDecl
	: public Decl
	, public SelfReference<VariadicDecl>
{
public:
	VariadicDecl()
		: Decl{}
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
		_node += " '" + Decl::ReturnType().TypeName() + "' ";
		_node += Decl::ReturnType()->StorageClassName();

		return _node;
	}

private:
	POLY_IMPL();
};

class FieldDecl
	: public Decl
	, public SelfReference<FieldDecl>
{
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

	virtual const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(FieldDecl).name()) };
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";
		_node += m_identifier;
		_node += " '" + Decl::ReturnType().TypeName() + "' ";
		_node += Decl::ReturnType()->StorageClassName();

		return _node;
	}

private:
	POLY_IMPL();
};

class RecordDecl
	: public Decl
	, public SelfReference<RecordDecl>
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

		ASTNode::UpdateDelegate();
	}

	virtual const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(RecordDecl).name()) };
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
		return std::string{ RemoveClassFromName(typeid(EnumConstantDecl).name()) } +" <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> " + m_identifier;
	}

private:
	POLY_IMPL();
};

class EnumDecl
	: public Decl
	, public SelfReference<EnumDecl>
{
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
	std::shared_ptr<ParamStmt> m_params;
	std::shared_ptr<CompoundStmt> m_body;
	std::weak_ptr<FunctionDecl> m_protoRef;
	std::vector<AST::TypeFacade> m_signature;

	bool m_isPrototype = true;
	size_t m_useCount = 0;

#if 0
private:
	auto IsUsed() const { return m_useCount > 0; }
#endif

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

	void SetParameterStatement(const std::shared_ptr<ParamStmt>& node)
	{
		assert(!m_params);

		ASTNode::AppendChild(NODE_UPCAST(node));
		m_params = node;

		ASTNode::UpdateDelegate();
	}

	auto IsPrototypeDefinition() const { return m_isPrototype; }
	auto HasPrototypeDefinition() const { return !m_protoRef.expired(); }

	// Bind function body to prototype definition
	void BindPrototype(const std::shared_ptr<FunctionDecl>& node)
	{
		assert(!m_isPrototype);
		assert(m_protoRef.expired());

		m_protoRef = node;
	}

#if 0
	void RegisterCaller()
	{
		m_useCount++;
	}
#endif

	const std::string NodeName() const
	{
		std::stringstream ss;
		ss << RemoveClassFromName(typeid(FunctionDecl).name());
		ss << " <line:" << line << ",col:" << col << "> ";

		if (IsPrototypeDefinition()) {
			ss << "proto ";
		}
#if 0
		else if (HasPrototypeDefinition()) {
			ss << "linked ";

			if (m_protoRef.lock()->IsUsed()) {
				ss << "used ";
			}
		}

		if (IsUsed()) {
			ss << "used ";
		}
#endif

		ss << m_identifier;

		if (!m_signature.empty()) {
			ss << " '" << Decl::ReturnType().TypeName() + " (";
			for (auto& type : m_signature) {
				ss << type.TypeName();
			}
			ss << ")' ";
		}
		else {
			ss << " '" << Decl::ReturnType().TypeName() << "' ";
		}

		ss << Decl::ReturnType()->StorageClassName() << " ";

		if (Decl::ReturnType()->IsInline()) {
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

	PRINT_NODE(TranslationUnitDecl);

private:
	POLY_IMPL();
};

//
// Expression nodes
//

class Expr : public ASTNode
{
protected:
	AST::TypeFacade m_returnType;

public:
	virtual ~Expr() = 0;

	auto& ReturnType() { return m_returnType; }
	auto& ReturnType() const { return m_returnType; }
};

class ResolveRefExpr
	: public Expr
	, public SelfReference<ResolveRefExpr>
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

private:
	POLY_IMPL();
};

class DeclRefExpr
	: public ResolveRefExpr
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
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";

		if (Expr::ReturnType().HasValue()) {
			_node += " '" + Expr::ReturnType().TypeName() + "' ";
			_node += Expr::ReturnType()->StorageClassName();
		}

		return _node;
	}

private:
	POLY_IMPL();
};

class BuiltinExpr final
	: public CallExpr
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

		ASTNode::UpdateDelegate();
	}

	const std::string NodeName() const final
	{
		return std::string{ RemoveClassFromName(typeid(BuiltinExpr).name()) } +" <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";
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

private:
	POLY_IMPL();
};

class ParenExpr
	: public Expr
	, public SelfReference<ParenExpr>
{
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

private:
	POLY_IMPL();
};

//
// Statement nodes
//

class Stmt : public ASTNode
{
public:
	virtual ~Stmt() = 0;
};

class ContinueStmt
	: public Stmt
	, public SelfReference<ContinueStmt>
{
public:
	PRINT_NODE(ContinueStmt);

private:
	POLY_IMPL();
};

class ReturnStmt
	: public Stmt
	, public SelfReference<ReturnStmt>
{
	std::shared_ptr<ASTNode> m_returnNode;

public:
	void SetReturnNode(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		m_returnNode = node;

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
	std::vector<std::shared_ptr<ASTNode>> m_arg;

public:
	void AppendArgument(std::shared_ptr<ASTNode>& node)
	{
		ASTNode::AppendChild(node);
		m_arg.push_back(node);

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

private:
	POLY_IMPL();
};

class CompoundStmt
	: public Stmt
	, public SelfReference<CompoundStmt>
{
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