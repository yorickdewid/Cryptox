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
	class Iterator : public ForwardItemTree
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

	class ConstIterator : public ForwardItemTree
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

public:
	using iterator = Iterator;
	using const_iterator = ConstIterator;

	// Move tree into AST wrapper
	AST(std::shared_ptr<ASTNode>&& tree)
		: m_tree{ std::move(tree) }
		, treeLink{ ++interfaceCounter }
	{
	}

	// Link new AST object to internal tree
	AST(std::shared_ptr<ASTNode>& tree)
		: m_tree{ tree }
		, treeLink{ ++interfaceCounter }
	{
	}

	// No implicit copy, only move
	AST(const AST&) = delete;
	AST(AST&&) = default;

	// Iterator interfaces
	iterator begin() { return Iterator{ m_tree }; }
	iterator end() { return Iterator{}; }
	const_iterator begin() const { return ConstIterator{ m_tree }; }
	const_iterator end() const { return ConstIterator{}; }
	const_iterator cbegin() const { return ConstIterator{ m_tree }; }
	const_iterator cend() const { return ConstIterator{}; }

	// Overload default swap via ADL
	void swap(std::shared_ptr<ASTNode>& rhs) noexcept
	{
		std::swap(m_tree, rhs);
	}

	// Capacity
	size_type size() { return std::distance(this->begin(), this->end()); }
	bool empty() { return std::distance(this->begin(), this->end()) == 0; }

	// Direct tree access
	inline ASTNode *operator->() const { return m_tree.get(); }

	// Copy self with new reference to tree
	AST tree_ref()
	{
		AST tmp{ m_tree };
		return tmp;
	}

	// AST tree reference trackers
	int TreeLinkId() const { return treeLink; }
	int TreeLinkCount() const { return interfaceCounter; }

private:
	std::shared_ptr<ASTNode> m_tree;
	const int treeLink;

private:
	static int interfaceCounter;
};

template<typename _Ty, typename... _Args>
inline auto MakeASTNode(_Args&&... args)
{
	auto ptr = std::make_shared<_Ty>(std::forward<_Args>(args)...);
	ptr->UpdateDelegate();
	return ptr;
}

} // namespace AST
} // namespace CoilCl

