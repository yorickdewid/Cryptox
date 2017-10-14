#pragma once

#include "ValueObject.h"

#include <list>
#include <memory>

#define PRINT_NODE(n) \
	virtual const std::string NodeName() const { \
		return std::string{ typeid(n).name() } + " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + ">"; \
	}

#define NODE_UPCAST(c) \
	std::dynamic_pointer_cast<ASTNode>(c)

class Decl;
class CompoundStmt;

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

	void Print(int level = 0);

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
	std::weak_ptr<ASTNode> parent;
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
		PLUS,
		EQ,
		MINUS,
	} m_operand;

	const char *BinOperandStr(BinOperand operand) const
	{
		switch (operand) {
		case BinaryOperator::PLUS:
			return "+";
		case BinaryOperator::EQ:
			return "==";
		case BinaryOperator::MINUS:
			return "-";
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
		return std::string{ typeid(BinaryOperator).name() } +" <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> 'return type' '" + BinOperandStr(m_operand) + "'";
	}
};

//
// Literal nodes
//

template<typename T>
class Literal : public ASTNode
{
protected:
	std::unique_ptr<ValueObject<T>> m_valueObj;

public:
	Literal()
		: m_valueObj{ new ValueObject<void>{Value::TypeSpecifier::T_VOID} }
	{
	}

	Literal(Value::TypeSpecifier tspec, T value)
		: m_valueObj{ new ValueObject<T>{tspec, value} }
	{
	}

	PRINT_NODE(Literal);
};

class StringLiteral : public Literal<std::string>
{
public:
	StringLiteral(const std::string& value)
		: Literal{ Value::TypeSpecifier::T_CHAR, value }
	{

	}

	PRINT_NODE(StringLiteral);
};

class IntegerLiteral : public Literal<int>
{
public:
	IntegerLiteral(int value)
		: Literal{ Value::TypeSpecifier::T_INT, value }
	{
	}

	PRINT_NODE(IntegerLiteral);
};

class FloatingLiteral : public Literal<double>
{
public:
	FloatingLiteral(double value)
		: Literal{ Value::TypeSpecifier::T_DOUBLE, value }
	{
	}

	PRINT_NODE(FloatingLiteral);
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

class CallExpr : public Expr
{
public:
	CallExpr()
		: Expr{}
	{
	}

	const std::string NodeName() const
	{
		return std::string{ typeid(CallExpr).name() } +" <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> 'return type'";
	}
};

class CastExpr : public Expr
{
	std::shared_ptr<ASTNode> rtype;
	bool implicit = true;

public:
	CastExpr()
		: Expr{}
	{
	}

	PRINT_NODE(CastExpr);
};

class DeclRefExpr : public Expr
{
	std::weak_ptr<Decl> m_ref; //TODO: VarDecl too strict?

public:
	DeclRefExpr(std::shared_ptr<Decl>& ref)
	{
		ASTNode::AppendChild(NODE_UPCAST(ref));
		m_ref = ref;
	}

	PRINT_NODE(DeclRefExpr);
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

	PRINT_NODE(Decl);
};

class VarDecl : public Decl
{
	std::shared_ptr<ASTNode> m_body;

public:
	VarDecl(const std::string& name, std::shared_ptr<ASTNode>& node)
		: Decl{ name }
		, m_body{ node }
	{
		ASTNode::AppendChild(node);
	}

	PRINT_NODE(VarDecl);
};

class FunctionDecl : public Decl
{
	std::shared_ptr<ASTNode> m_params;
	std::shared_ptr<CompoundStmt> m_body; //TODO: CompoundStmt not always the case

public:
	//TODO: type
	FunctionDecl(const std::string& name, std::shared_ptr<ASTNode>& node)
		: Decl{ name }
		, m_body{ std::dynamic_pointer_cast<CompoundStmt>(node) }
	{
		ASTNode::AppendChild(node);
	}

	const std::string NodeName() const
	{
		return std::string{ typeid(FunctionDecl).name() } +" <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> " + m_identifier + " 'return type'";
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
// Statement nodes
//

class Stmt : public ASTNode
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
	std::shared_ptr<CompoundStmt> truthStmt;
	std::shared_ptr<CompoundStmt> altStmt;

public:
	IfStmt(std::shared_ptr<ASTNode>& eval, std::shared_ptr<CompoundStmt> truth = nullptr, std::shared_ptr<CompoundStmt> alt = nullptr)
		: evalNode{ eval }
	{
		ASTNode::AppendChild(eval);

		if (truth) {
			ASTNode::AppendChild(NODE_UPCAST(truth));
			truthStmt = truth;
		}

		if (alt) {
			ASTNode::AppendChild(NODE_UPCAST(alt));
			altStmt = alt;
		}
	}

	PRINT_NODE(IfStmt);
};

class DeclStmt : public Stmt
{
	std::shared_ptr<VarDecl> m_value;

public:
	DeclStmt(std::shared_ptr<VarDecl>& value)
		: m_value{ value }
	{
		ASTNode::AppendChild(NODE_UPCAST(value));
	}

	PRINT_NODE(DeclStmt);
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