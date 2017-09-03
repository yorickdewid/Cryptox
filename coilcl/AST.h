#pragma once

#include "ValueObject.h"

#include <list>
#include <memory>

class ASTNode;

class AST
{
	std::unique_ptr<ASTNode> root = nullptr;

public:
	AST();
	~AST();

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

	inline bool IsRoot() const
	{
		return parent == nullptr;
	}
};

class ValueNode : public ASTNode
{
	std::unique_ptr<Value> value;

public:
	ValueNode(std::unique_ptr<Value> _value)
	{
		value = std::move(_value);
	}

	ValueNode()
		: value{ new ValueObject<void>(Value::TypeSpecifier::T_VOID) }
	{
	}
};

class FunctionNode : public ASTNode
{
	std::string funcName;
	std::unique_ptr<ASTNode> returnType;
	std::list<std::unique_ptr<ValueNode>> parameters;

public:
	FunctionNode(const std::string& name)
		: funcName{ name }
	{
	}

	FunctionNode(std::shared_ptr<Value> value)
	{
		assert(value->DataType() == Value::TypeSpecifier::T_CHAR);

		if (value->IsArray()) {
			funcName = value->As<std::string>();
		}
		else {
			funcName[0] = value->As<char>();
		}
	}

	void ReturnType(std::unique_ptr<ASTNode> &rtnVal)
	{
		returnType = std::move(rtnVal);
	}
};

