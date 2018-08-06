// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/AST/ASTNode.h>

namespace CryCC
{
namespace AST
{

class ForwardItemTree
{
protected:
	ForwardItemTree() = default;

	static void ForwardInternalTree(ASTNodeType& node);
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
		ASTNodeType cNode;

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
		Iterator(const ASTNodeType& node) : cNode{ node } {}
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
		ASTNodeType cNode;

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
		ConstIterator(const ASTNodeType& node) : cNode{ node } {}
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

	//FUTURE: BidirectionalIterator

public:
	using iterator = Iterator;
	using const_iterator = ConstIterator;

	// Wrapper without tree.
	AST() = default;
	AST(nullptr_t)
	{
	}

	// Move tree into AST wrapper.
	template<typename Node>
	AST(std::shared_ptr<Node>&& tree)
		: m_tree{ std::move(tree) }
	{
	}

	// Link new AST object to internal tree.
	template<typename Node>
	AST(const std::shared_ptr<Node>& tree)
		: m_tree{ tree }
	{
	}

	// Iterator interfaces.
	iterator Begin() { return Iterator{ m_tree }; }
	iterator End() { return Iterator{}; }
	const_iterator Begin() const { return ConstIterator{ m_tree }; }
	const_iterator End() const { return ConstIterator{}; }
	const_iterator Cbegin() const { return ConstIterator{ m_tree }; }
	const_iterator Cend() const { return ConstIterator{}; }

	// Iterator interfaces.
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

	// Overload default swap via ADL.
	void swap(ASTNodeType& rhs) noexcept { Swap(rhs); }
	void Swap(ASTNodeType& rhs) noexcept
	{
		std::swap(m_tree, rhs);
	}

	// Capacity.
	size_type size() const { return Size(); }
	size_type Size() const
	{
		if (!m_tree) { return 0; }
		return std::distance(this->cbegin(), this->cend());
	}

	// Is empty check.
	bool empty() const { return Empty(); }
	bool Empty() const
	{
		if (!m_tree) { return false; }
		return std::distance(this->cbegin(), this->cend()) == 0;
	}

	//TODO: limit access or remove?
	// Direct tree access
	inline ASTNode *operator->() const noexcept { return m_tree.get(); }

	//TODO: limit access or remove ?
	//TODO: this is not wat we want since this allows undefined ownership
	// Get top node
	inline ASTNode *operator*() const noexcept { return m_tree.get(); }

	//TODO: Remove this method in favor of copy ctor
	// Copy self with new reference to tree
	[[deprecated]]
	AST tree_ref()
	{
		AST copy{ m_tree };
		return copy;
	}

	//FUTURE: DeepCopy()

private:
	ASTNodeType m_tree;
};

} // namespace AST
} // namespace CryCC
