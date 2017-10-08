#pragma once

#include "ValueObject.h"

#include <list>
#include <memory>

#define PRINT_NODE(n) \
	const std::string NodeName() const { \
		return std::string{ typeid(n).name() } + " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + ">"; \
	}

class VarDecl;
class CompoundStmt;

class ASTNode
{
protected:
	int line = -1;
	int col = -1;

public:
	ASTNode() = default;

	inline size_t Children() const
	{
		return children.size();
	}

	virtual const std::string NodeName() const = 0;

protected:
	virtual void AppendChild(std::shared_ptr<ASTNode>& node)
	{
		children.push_back(node);
	}

protected:
	std::list<std::weak_ptr<ASTNode>> children;
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
public:
	enum Op // move ?
	{
		PLUS,
		EQ,
	};

	PRINT_NODE(BinaryOperator);
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

	PRINT_NODE(CallExpr);
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
	std::weak_ptr<VarDecl> m_ref; //TODO: VarDecl too strict?

public:
	DeclRefExpr(std::shared_ptr<VarDecl>& ref)
	{
		m_ref = ref;
	}

	PRINT_NODE(DeclRefExpr);
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
public:
	VarDecl(const std::string& name)
		: Decl{ name }
	{
	}

	PRINT_NODE(VarDecl);
};

class FunctionDecl : public Decl
{
	std::shared_ptr<ASTNode> m_params;

public:
	//TODO: type
	FunctionDecl(const std::string& name)
		: Decl{ name }
	{
	}

	PRINT_NODE(FunctionDecl);

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

class TranslationUnitDecl : public Decl
{
public:
	TranslationUnitDecl(const std::string& sourceName)
		: Decl{ sourceName }
	{
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
	{
		ASTNode::AppendChild(eval);
		evalNode = eval;

		if (truth) {
			ASTNode::AppendChild(std::dynamic_pointer_cast<ASTNode>(truth));
			truthStmt = truth;
		}

		if (alt) {
			ASTNode::AppendChild(std::dynamic_pointer_cast<ASTNode>(alt));
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
		ASTNode::AppendChild(std::dynamic_pointer_cast<ASTNode>(value));
	}

	PRINT_NODE(DeclStmt);
};

class CompoundStmt : public Stmt
{
public:
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