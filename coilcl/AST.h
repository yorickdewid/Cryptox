#pragma once

#include "ValueObject.h"

#include <list>
#include <vector>
#include <memory>

#define PRINT_NODE(n) \
	virtual const std::string NodeName() const { \
		return std::string{ RemoveClassFromName(typeid(n).name()) } + " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + ">"; \
	}

#define NODE_UPCAST(c) \
	std::dynamic_pointer_cast<ASTNode>(c)

template<typename _Ty>
std::string RemoveClassFromName(_Ty *_name)
{
	constexpr const char stripBlockStr[] = "::";
	std::string f{ _name };
	if (size_t pos = f.rfind(stripBlockStr) != std::string::npos) {
		return f.substr(pos + sizeof(stripBlockStr) - 1);
	}
	constexpr const char stripClassStr[] = "class";
	if (size_t pos = f.find_last_of(stripClassStr) != std::string::npos) {
		return f.substr(pos + sizeof(stripClassStr) - 1);
	}
	return f;
}

class DeclRefExpr;
class CompoundStmt;
class ArgumentStmt;

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

		XOR,		// ^
		AND,		// &

		SLEFT,		// <<
		SRIGHT,		// >>

		EQ,			// ==
		NEQ			// !=
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

namespace CoilCl
{
namespace AST
{

class UnaryOperator : public Operator
{
	std::shared_ptr<DeclRefExpr> m_body;

public:
	enum UnaryOperand
	{
		INC,		// ++
		DEC			// --
	} m_operand;

	const char *UnaryOperandStr(UnaryOperand operand) const
	{
		switch (operand) {
		case UnaryOperand::INC:
			return "++";
		case UnaryOperand::DEC:
			return "--";
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
	UnaryOperator(UnaryOperand operand, OperandSide side, std::shared_ptr<DeclRefExpr>& node)
		: m_operand{ operand }
		, m_side{ side }
	{
		ASTNode::AppendChild(NODE_UPCAST(node));
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

//
// Literal nodes
//

template<typename T>
class Literal : public ASTNode
{
protected:
	std::shared_ptr<ValueObject<T>> m_valueObj;

public:
	// Default to void type with no data
	Literal()
		: m_valueObj{ new ValueObject<void>{Value::TypeSpecifier::T_VOID} }
	{
	}

	// Move data object from lexer into literal
	Literal(std::shared_ptr<ValueObject<T>>&& object)
		: m_valueObj{ std::move(object) }
	{
	}

	PRINT_NODE(Literal);
};

class CharacterLiteral : public Literal<char>
{
public:
	template<typename _Ty>
	CharacterLiteral(_Ty&& value)
		: Literal{ std::forward<_Ty>(value) }
	{
	}

	const std::string NodeName() const
	{
		return std::string{ RemoveClassFromName(typeid(CharacterLiteral).name()) } +" <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> " + m_valueObj->ToString();
	}
};

class StringLiteral : public Literal<std::string>
{
public:
	template<typename _Ty>
	StringLiteral(_Ty&& value)
		: Literal{ std::forward<_Ty>(value) }
	{
	}

	const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(StringLiteral).name()) };
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";
		_node += "'const char [" + std::to_string(m_valueObj->Size()) + "]' ";
		_node += "\"" + m_valueObj->ToString() + "\"";
		return _node;
	}
};

class IntegerLiteral : public Literal<int>
{
public:
	template<typename _Ty>
	IntegerLiteral(_Ty&& value)
		: Literal{ std::forward<_Ty>(value) }
	{
	}

	const std::string NodeName() const
	{
		return std::string{ RemoveClassFromName(typeid(IntegerLiteral).name()) } +" <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> " + m_valueObj->ToString();
	}
};

class FloatingLiteral : public Literal<double>
{
public:
	template<typename _Ty>
	FloatingLiteral(_Ty&& value)
		: Literal{ std::forward<_Ty>(value) }
	{
	}

	PRINT_NODE(FloatingLiteral);
};

//
// Declaration nodes
//

class Decl : public ASTNode
{
protected:
	std::string m_identifier;
	std::shared_ptr<ASTNode> m_returnType;

public:
	Decl(const std::string& name)
		: m_identifier{ name }
	{
	}

	auto Identifier() const
	{
		return m_identifier;
	}

	PRINT_NODE(Decl);
};

class VarDecl : public Decl
{
	std::shared_ptr<ASTNode> m_body;

public:
	VarDecl(const std::string& name, std::shared_ptr<ASTNode> node = nullptr)
		: Decl{ name }
		, m_body{ node }
	{
		ASTNode::AppendChild(node);
	}

	virtual const std::string NodeName() const
	{
		return std::string{ RemoveClassFromName(typeid(VarDecl).name()) } +" <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> " + m_identifier;
	}
};

class FieldDecl : public Decl
{
public:
	FieldDecl(const std::string& name)
		: Decl{ name }
	{
	}

	PRINT_NODE(FieldDecl);
};

class RecordDecl : public Decl
{
public:
	RecordDecl(const std::string& name)
		: Decl{ name }
	{
	}

	PRINT_NODE(RecordDecl);
};

class FunctionDecl : public Decl
{
	std::shared_ptr<ASTNode> m_params;//TODO: Add parameters
	std::shared_ptr<CompoundStmt> m_body; //TODO: CompoundStmt not always the case
	std::weak_ptr<FunctionDecl> m_protoRef;
	bool m_isPrototype = false;
	bool m_isReferenced = false;

	static std::vector<std::weak_ptr<FunctionDecl>> m_crossResolveList;

public:
	//TODO: type
	FunctionDecl(const std::string& name, std::shared_ptr<ASTNode>& node)
		: Decl{ name }
		, m_body{ std::dynamic_pointer_cast<CompoundStmt>(node) }
	{
		ASTNode::AppendChild(node);
	}

	// Constructor only used for prototype function definitions
	FunctionDecl(const std::string& name)
		: Decl{ name }
		, m_isPrototype{ true }
	{
		//m_crossResolveList.push_back()
	}

	auto IsPrototypeDefinition() const
	{
		return m_isPrototype;
	}

	const std::string NodeName() const
	{
		std::string _node{ RemoveClassFromName(typeid(FunctionDecl).name()) };
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";

		if (IsPrototypeDefinition()) {
			_node += "proto ";
		}
		else if (!m_protoRef.expired()) {
			_node += "used ";
		}
		if (m_isReferenced) {
			_node += "used ";
		}

		_node += m_identifier;
		_node += " 'return type'";

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
protected:
	void BindPrototype(std::shared_ptr<FunctionDecl>& node)
	{
		assert(node->IsPrototypeDefinition());

		node->m_isReferenced = true;
		m_protoRef = node;
	}
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

class DeclRefExpr : public Expr
{
	std::weak_ptr<Decl> m_ref;

public:
	// We're not saving the reference as child in the root to prevent
	// circulair references in the upper node.
	DeclRefExpr(std::shared_ptr<Decl>& ref)
	{
		m_ref = ref;
	}

	const std::string NodeName() const
	{
		return std::string{ RemoveClassFromName(typeid(DeclRefExpr).name()) } +" <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> linked '" + m_ref.lock()->Identifier() + "'";
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

//
// Statement nodes
//

class Stmt : public ASTNode
{
public:
	PRINT_NODE(Stmt);
};

class NullStmt : public Stmt
{
public:
	PRINT_NODE(Stmt);
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

template<typename _Ty, typename _Decl = DeclRefExpr> inline
std::shared_ptr<_Decl> make_ref(_Ty&& _Args)
{
	return std::make_shared<_Decl>(_Decl{ _Args });
}
