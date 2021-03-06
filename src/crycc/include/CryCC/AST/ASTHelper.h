// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/AST/ASTNode.h>
#include <CryCC/AST/ASTTrait.h>

namespace CryCC
{
namespace AST
{

namespace Compare
{

template<typename NodeType, typename BaseType = ASTNode>
struct Equal
{
	bool operator()(BaseType& item)
	{
		return typeid(item) == typeid(NodeType);
	}
};

template<typename NodeType, typename BaseType = ASTNode>
struct Derived
{
	bool operator()(BaseType& item)
	{
		return dynamic_cast<NodeType*>(&item) != nullptr;
	}
};

template<typename _Cmp, typename... _VariaTy>
struct CombinedOrImpl;

template<typename _Cmp, typename _FirstTy, typename... _VariaTy>
struct CombinedOrImpl<_Cmp, _FirstTy, _VariaTy...>
{
	bool operator()(_Cmp& item)
	{
		return Equal<_FirstTy>{}(item) || CombinedOrImpl<_Cmp, _VariaTy...>{}(item);
	}
};

template<typename _Cmp, typename _LastTy>
struct CombinedOrImpl<_Cmp, _LastTy>
{
	bool operator()(_Cmp& item)
	{
		return Equal<_LastTy>{}(item);
	}
};

template<typename... _VariaTy>
using CombinedOr = CombinedOrImpl<ASTNode, _VariaTy...>;

template<typename _Cmp, typename... _VariaTy>
struct MultiDeriveImpl;

template<typename _Cmp, typename _FirstTy, typename... _VariaTy>
struct MultiDeriveImpl<_Cmp, _FirstTy, _VariaTy...>
{
	bool operator()(_Cmp& item)
	{
		return Derived<_FirstTy>{}(item) || MultiDeriveImpl<_Cmp, _VariaTy...>{}(item);
	}
};

template<typename _Cmp, typename _LastTy>
struct MultiDeriveImpl<_Cmp, _LastTy>
{
	bool operator()(_Cmp& item)
	{
		return Derived<_LastTy>{}(item);
	}
};

template<typename... _VariaTy>
using MultiDerive = MultiDeriveImpl<ASTNode, _VariaTy...>;

} // namespace Compare

// Find the closest node in the parent chain. If the root node is reached and no match is found
// the function returns a nullptr. Otherwise a shared pointer to the node is returned.
template<typename NodeType, typename = typename std::enable_if<IsASTNode<NodeType>::value>::type>
std::shared_ptr<NodeType> Closest(const std::shared_ptr<ASTNode>& node)
{
	Compare::Equal<NodeType> eqOp;
	if (const ASTNodeType parent = node->Parent().lock()) {
		if (!eqOp(*parent.get())) {
			return Closest<NodeType>(parent);
		}

		return Util::NodeCast<NodeType>(parent);
	}

	return nullptr;
}

} // namespace AST
} // namespace CryCC

namespace Util
{

using namespace CryCC::AST;

// Create a new node instance and list this node as the parent node for each child.
// Child nodes can already exist if they are passed in the constructor of the node.
template<typename NodeType, typename... ArgTypes>
inline auto MakeASTNode(ArgTypes&&... args)
{
	auto ptr = std::make_shared<NodeType>(std::forward<ArgTypes>(args)...);
	ptr->UpdateDelegate();
	return ptr;
}

// Create a new unit scoped tree. This is the recommended manner to create a new tree.
template<typename... ArgTypes>
inline auto MakeUnitTree(ArgTypes&&... args)
{
	return MakeASTNode<TranslationUnitDecl>(std::forward<ArgTypes>(args)...);
}

// Test if the AST node is a literal node.
template<typename NodeType, typename = typename std::enable_if<std::is_base_of<ASTNode, NodeType>::value
	|| std::is_same<NodeType, ASTNode>::value>::type>
	bool IsNodeLiteral(const std::shared_ptr<NodeType>& type)
{
	//TODO: poor man's solution
	switch (type->Label())
	{
	case NodeID::CHARACTER_LITERAL_ID:
	case NodeID::STRING_LITERAL_ID:
	case NodeID::INTEGER_LITERAL_ID:
	case NodeID::FLOAT_LITERAL_ID:
		return true;
	}

	return false;
}
template<typename NodeType, typename = typename std::enable_if<std::is_base_of<ASTNode, NodeType>::value
	|| std::is_same<NodeType, ASTNode>::value>::type>
	inline bool IsNodeFunction(const std::shared_ptr<NodeType>& type)
{
	return type->Label() == NodeID::FUNCTION_DECL_ID;
}
template<typename NodeType, typename = typename std::enable_if<std::is_base_of<ASTNode, NodeType>::value
	|| std::is_same<NodeType, ASTNode>::value>::type>
	inline bool IsNodeTranslationUnit(const std::shared_ptr<NodeType>& type)
{
	return type->Label() == NodeID::TRANSLATION_UNIT_DECL_ID;
}
template<typename NodeType, typename = typename std::enable_if<std::is_base_of<ASTNode, NodeType>::value
	|| std::is_same<NodeType, ASTNode>::value>::type>
	inline bool IsNodeCompound(const std::shared_ptr<NodeType>& type)
{
	return type->Label() == NodeID::COMPOUND_STMT_ID;
}

template<typename CastNode>
auto NodeCast(const std::shared_ptr<ASTNode>& node)
{
	return std::dynamic_pointer_cast<CastNode>(node);
}
template<typename CastNode>
auto NodeCast(const std::weak_ptr<ASTNode>& node)
{
	if (node.expired()) {
		throw 1; //TODO: some bad ptr exception
	}
	return NodeCast<CastNode>(node.lock());
}
template<typename CastNode>
auto NodeCast(CastNode) = delete;

} // namespace Util
