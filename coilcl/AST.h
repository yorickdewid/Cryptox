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
		else {
			//? ?
		}
	}
};

class ASTNode
{
	std::unique_ptr<ASTNode> parent = nullptr;
	std::list<std::unique_ptr<ASTNode>> children;

public:
	ASTNode() = default;

	inline size_t Children()
	{
		return children.size();
	}

	void AppendChild(std::unique_ptr<ASTNode> node)
	{
		auto child = std::move(node);
		child->parent.reset(this);
		children.push_back(std::move(child));
	}

	inline bool IsRoot() const
	{
		return parent == nullptr;
	}
};

class FunctionNode : public ASTNode
{
	std::string funcName;
	std::unique_ptr<Value> returnType;
	std::list<std::unique_ptr<Value>> parameters;

public:
	FunctionNode(const std::string& name)
		: funcName{ name }
		, returnType{ new ValueObject<void>(Value::TypeSpecifier::T_VOID) }
	{
	}

	FunctionNode(std::shared_ptr<Value> value)
		: returnType{ new ValueObject<void>(Value::TypeSpecifier::T_VOID) }
	{
		assert(value->DataType() == Value::TypeSpecifier::T_CHAR);

		if (value->IsArray()) {
			funcName = value->As<std::string>();
		}
		else {
			funcName[0] = value->As<char>();
		}
	}

	void ReturnType(std::unique_ptr<Value> rtnVal)
	{
		returnType = std::move(rtnVal);
	}
};

