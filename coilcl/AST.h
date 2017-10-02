#pragma once

#include "ValueObject.h"

#include <map>
#include <list>
#include <memory>
#include <functional>

class ASTNode;
class FunctionNode;

class AST
{
	std::map<std::string, FunctionNode> functionTable;
	std::unique_ptr<ASTNode> root = nullptr;

public:
	void PushNode(std::unique_ptr<ASTNode> node)
	{
		// Add new root if root is empty
		if (root == nullptr) {
			root = std::move(node);
		}
	}
};

class ASTNode
{
	std::unique_ptr<ASTNode> parent = nullptr;
	//std::list<std::unique_ptr<ASTNode>> children;

public:
	ASTNode() = default;

	/*inline size_t Children()
	{
		return children.size();
	}

	void AppendChild(std::unique_ptr<ASTNode> node)
	{
		auto child = std::move(node);
		child->parent.reset(this);
		children.push_back(std::move(child));
	}*/

	inline auto IsRoot() const { return parent == nullptr; }
	inline auto IsCallable() const { return false; }
};

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

class FunctionNode : public ASTNode
{
	std::string name;
	std::unique_ptr<ASTNode> returnType;
	std::list<std::unique_ptr<ValueNode>> parameters;

public:
	FunctionNode(const std::string& name)
		: name{ name }
	{
	}

	FunctionNode(std::unique_ptr<Value>& value)
	{
		assert(value->DataType() == Value::TypeSpecifier::T_CHAR);

		if (value->IsArray()) {
			name = value->As<std::string>();
		}
		else {
			name.push_back(value->As<char>());
		}
	}

	void ReturnType(std::unique_ptr<ASTNode> rtnVal)
	{
		returnType = std::move(rtnVal);
	}
};

class BinaryOp : public ASTNode
{

};

class ExpressionCall : public ASTNode
{
public:
	ExpressionCall()
	{

	}
};

class ExpressionCast : public ASTNode
{
public:
	ExpressionCast()
	{

	}
};

class IfStatementNode : public ASTNode
{
public:
	IfStatementNode()
	{

	}
};
