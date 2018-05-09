// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "ASTNode.h"

namespace CoilCl
{
namespace AST
{

namespace Compare
{

template<typename _Ty, typename _Cmp = ASTNode>
struct Equal
{
	bool operator()(_Cmp& item)
	{
		return typeid(item) == typeid(_Ty);
	}
};

template<typename _Ty, typename _Cmp = ASTNode>
struct Derived
{
	bool operator()(_Cmp& item)
	{
		return dynamic_cast<_Ty*>(&item) != nullptr;
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

class ForwardItemTree
{
protected:
	ForwardItemTree() = default;

	static void ForwardInternalTree(std::shared_ptr<ASTNode>& node);
};

// The AST class provides a wrapper around the tree and the tree
// node operations. All interfaces interacting with the tree from
// a public accessor should use this single interface to modify the
// tree internals.
class AST
{
	using _MyTy = AST;
	using _ValTy = ASTNode;

public: // Member types
	using value_type = _ValTy;
	using reference = value_type & ;
	using const_reference = const value_type&;
	using pointer = value_type * ;
	using const_pointer = const value_type*;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

public:
	class Iterator : private ForwardItemTree
	{
		std::shared_ptr<ASTNode> cNode;

	private:
		using _MyTy = Iterator;

	public:
		using value_type = _ValTy;
		using reference = value_type & ;
		using pointer = value_type * ;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::forward_iterator_tag;

		// Constructors
		Iterator() : cNode{ nullptr } {}
		Iterator(const std::shared_ptr<ASTNode>& node) : cNode{ node } {}
		Iterator(const Iterator&) = default;

		_MyTy& operator++()
		{
			ForwardItemTree::ForwardInternalTree(std::ref(cNode));
			return (*this);
		}

		_MyTy operator++(int)
		{
			_MyTy tmp = (*this);
			++(*this);
			return tmp;
		}

		// Iterator element access
		reference operator*() { return *(cNode.get()); }
		pointer operator->() { return cNode.get(); }

		auto shared_ptr() -> decltype(cNode) const
		{
			return cNode;
		}

		bool operator==(const Iterator &other) const { return cNode == other.cNode; }
		bool operator!=(const Iterator &other) const { return cNode != other.cNode; }
		bool operator<(const Iterator& other) const { return cNode < other.cNode; }
		bool operator>(const Iterator& other) const { return cNode > other.cNode; }
		bool operator<=(const Iterator& other) const { return cNode <= other.cNode; }
		bool operator>=(const Iterator& other) const { return cNode >= other.cNode; }
	};

	class ConstIterator : private ForwardItemTree
	{
		std::shared_ptr<ASTNode> cNode;

	private:
		using _MyTy = ConstIterator;

	public:
		using value_type = _ValTy;
		using reference = value_type & ;
		using pointer = value_type * ;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::forward_iterator_tag;

		// Constructors
		ConstIterator() : cNode{ nullptr } {}
		ConstIterator(const std::shared_ptr<ASTNode>& node) : cNode{ node } {}
		ConstIterator(const ConstIterator&) = default;

		const _MyTy& operator++()
		{
			ForwardItemTree::ForwardInternalTree(std::ref(cNode));
			return (*this);
		}

		const _MyTy operator++(int)
		{
			_MyTy tmp = (*this);
			++(*this);
			return tmp;
		}

		// Iterator element access
		reference operator*() { return *(cNode.get()); }
		pointer operator->() { return cNode.get(); }

		auto shared_ptr() -> decltype(cNode) const
		{
			return cNode;
		}

		bool operator==(const ConstIterator &other) const { return cNode == other.cNode; }
		bool operator!=(const ConstIterator &other) const { return cNode != other.cNode; }
		bool operator<(const ConstIterator& other) const { return cNode < other.cNode; }
		bool operator>(const ConstIterator& other) const { return cNode > other.cNode; }
		bool operator<=(const ConstIterator& other) const { return cNode <= other.cNode; }
		bool operator>=(const ConstIterator& other) const { return cNode >= other.cNode; }
	};

	//TODO: BidirectionalIterator

public:
	using iterator = Iterator;
	using const_iterator = ConstIterator;

	// Wrapper without tree
	AST() = default;
	AST(nullptr_t)
	{
	}

	// Move tree into AST wrapper
	template<typename Node>
	AST(std::shared_ptr<Node>&& tree)
		: m_tree{ std::move(tree) }
	{
	}

	// Link new AST object to internal tree
	template<typename Node>
	AST(const std::shared_ptr<Node>& tree)
		: m_tree{ tree }
	{
	}

	// Iterator interfaces
	iterator Begin() { return Iterator{ m_tree }; }
	iterator End() { return Iterator{}; }
	const_iterator Begin() const { return ConstIterator{ m_tree }; }
	const_iterator End() const { return ConstIterator{}; }
	const_iterator Cbegin() const { return ConstIterator{ m_tree }; }
	const_iterator Cend() const { return ConstIterator{}; }

	// Iterator interfaces
	iterator begin() { return Begin(); }
	iterator end() { return End(); }
	const_iterator begin() const { return Begin(); }
	const_iterator end() const { return End(); }
	const_iterator cbegin() const { return Cbegin(); }
	const_iterator cend() const { return Cend(); }

	reference front() { return Front(); }
	reference Front()
	{
		return (*m_tree.get());
	}

	// Overload default swap via ADL
	void swap(std::shared_ptr<ASTNode>& rhs) noexcept { Swap(rhs); }
	void Swap(std::shared_ptr<ASTNode>& rhs) noexcept
	{
		std::swap(m_tree, rhs);
	}

	// Capacity
	size_type size() const { return Size(); }
	size_type Size() const
	{
		if (!m_tree) { return 0; }
		return std::distance(this->cbegin(), this->cend());
	}

	// Is empty check
	bool empty() const { return Empty(); }
	bool Empty() const
	{
		if (!m_tree) { return false; }
		return std::distance(this->cbegin(), this->cend()) == 0;
	}

	//[[deprecated]] // TODO: remove
	inline bool has_tree() const noexcept { return !!m_tree; }

	//TODO: limit access or remove?
	// Direct tree access
	inline ASTNode *operator->() const noexcept { return m_tree.get(); }

	//TODO: limit access or remove ?
	//TODO: this is not wat we want since this allows undefined ownership
	// Get top node
	inline ASTNode *operator*() const noexcept { return m_tree.get(); }

	//TODO: Remove this method in favor of copy ctor
	// Copy self with new reference to tree
	//[[deprecated]]
	AST tree_ref()
	{
		AST copy{ m_tree };
		return copy;
	}

	//TODO: DeepCopy()

private:
	std::shared_ptr<ASTNode> m_tree;
};

template<typename _Ty, typename... _Args>
inline auto MakeASTNode(_Args&&... args)
{
	auto ptr = std::make_shared<_Ty>(std::forward<_Args>(args)...);
	ptr->UpdateDelegate();
	return ptr;
}

} // namespace AST

namespace Util
{

template<typename NodeType, typename = typename std::enable_if<std::is_convertible<NodeType, AST::ASTNode>::value
	|| std::is_same<NodeType, AST::ASTNode>::value>::type>
bool IsNodeLiteral(const std::shared_ptr<NodeType>& type)
{
	//TODO: poor man's solution
	switch (type->Label())
	{
	case AST::NodeID::CHARACTER_LITERAL_ID:
	case AST::NodeID::STRING_LITERAL_ID:
	case AST::NodeID::INTEGER_LITERAL_ID:
	case AST::NodeID::FLOAT_LITERAL_ID:
		return true;
	}

	return false;
}
template<typename NodeType, typename = typename std::enable_if<std::is_convertible<NodeType, AST::ASTNode>::value
	|| std::is_same<NodeType, AST::ASTNode>::value>::type>
inline bool IsNodeFunction(const std::shared_ptr<NodeType>& type)
{
	return type->Label() == AST::NodeID::FUNCTION_DECL_ID;
}
template<typename NodeType, typename = typename std::enable_if<std::is_convertible<NodeType, AST::ASTNode>::value
	|| std::is_same<NodeType, AST::ASTNode>::value>::type>
inline bool IsNodeTranslationUnit(const std::shared_ptr<NodeType>& type)
{
	return type.Label() == AST::NodeID::TRANSLATION_UNIT_DECL_ID;
}

template<typename CastNode>
auto NodeCast(const std::shared_ptr<AST::ASTNode>& node)
{
	return std::static_pointer_cast<CastNode>(node);
}
template<typename CastNode>
auto NodeCast(const std::weak_ptr<AST::ASTNode>& node)
{
	if (node.expired()) {
		throw 1; //TODO: some bad ptr exception
	}
	return NodeCast<CastNode>(node.lock());
}

} // namespace Util
} // namespace CoilCl

