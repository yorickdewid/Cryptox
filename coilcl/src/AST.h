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

template<typename _Ty>
struct ASTEqual
{
	bool operator()(ASTNode& item)
	{
		return typeid(item) == typeid(_Ty);
	}
};

class AST
{
	using _MyTy = AST;
	using _ValTy = ASTNode;

public: // Member types
	using value_type = _ValTy;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

public:
	class Iterator
	{
		std::shared_ptr<ASTNode> cNode;

	private:
		using _MyTy = Iterator;

	private:
		void IncrementInternalTree()
		{
			// No children in this node, work upwards and sideways
			if (cNode->ChildrenCount() == 0) {
				auto weakParent = cNode->Parent();
				if (auto parent = weakParent.lock()) {
					auto parentChildren = parent->Children();

					auto selfListItem = std::find_if(parentChildren.begin(), parentChildren.end(), [=](std::weak_ptr<ASTNode>& wPtr)
					{
						return wPtr.lock() == cNode;
					});

					if (selfListItem + 1 == parentChildren.end()) {
						cNode = nullptr;
					}
					else {
						auto weakNeighbour = selfListItem + 1;
						if (auto neighbour = weakNeighbour->lock()) {
							cNode = neighbour;
						}
					}
				}
				else {
					cNode = nullptr;
				}
			}
			else {
				auto firstWeakChild = cNode->At(0);
				if (auto firstChild = firstWeakChild.lock()) {
					cNode = firstChild;
				}
				else {
					cNode = nullptr;
				}
			}
		}

	public:
		using value_type = _ValTy;
		using reference = value_type&;
		using pointer = value_type*;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::forward_iterator_tag;

		// Constructors
		Iterator() : cNode{ nullptr } {}
		Iterator(const std::shared_ptr<ASTNode>& node) : cNode{ node } {}
		Iterator(const Iterator&) = default;

		_MyTy& operator++()
		{
			IncrementInternalTree();
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

		//Iterator& operator=(const Iterator& other) { return Iterator{ other }; }
		bool operator==(const Iterator &other) const { return cNode == other.cNode; }
		bool operator!=(const Iterator &other) const { return cNode != other.cNode; }
		bool operator<(const Iterator& other) const { return cNode < other.cNode; }
		bool operator>(const Iterator& other) const { return cNode > other.cNode; }
		bool operator<=(const Iterator& other) const { return cNode <= other.cNode; }
		bool operator>=(const Iterator& other) const { return cNode >= other.cNode; }
	};

	class ConstIterator
	{
		std::shared_ptr<ASTNode> cNode;

	private:
		using _MyTy = ConstIterator;

	private:
		void IncrementInternalTree()
		{
			// No children in this node, work upwards and sideways
			if (cNode->ChildrenCount() == 0) {
			redo:
				auto weakParent = cNode->Parent();
				if (auto parent = weakParent.lock()) {
					auto parentChildren = parent->Children();

					auto selfListItem = std::find_if(parentChildren.begin(), parentChildren.end(), [=](std::weak_ptr<ASTNode>& wPtr)
					{
						return wPtr.lock() == cNode;
					});

					if (selfListItem + 1 == parentChildren.end()) {
						cNode = parent;
						goto redo;
					}
					else {
						auto weakNeighbour = selfListItem + 1;
						if (auto neighbour = weakNeighbour->lock()) {
							cNode = neighbour;
						}
					}
				}
				else {
					cNode = nullptr;
				}
			}
			else {
				auto firstWeakChild = cNode->At(0);
				if (auto firstChild = firstWeakChild.lock()) {
					cNode = firstChild;
				}
				else {
					cNode = nullptr;
				}
			}
		}

	public:
		using value_type = _ValTy;
		using reference = value_type&;
		using pointer = value_type*;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::forward_iterator_tag;

		// Constructors
		ConstIterator() : cNode{ nullptr } {}
		ConstIterator(const std::shared_ptr<ASTNode>& node) : cNode{ node } {}
		ConstIterator(const ConstIterator&) = default;

		const _MyTy& operator++()
		{
			IncrementInternalTree();
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

	AST(std::shared_ptr<ASTNode>&& tree)
		: m_tree{ std::move(tree) }
	{
	}

	AST(std::shared_ptr<ASTNode>& tree)
		: m_tree{ tree }
	{
	}

	// No implicit copy, only move
	AST(const AST&) = delete;
	AST(AST&&) = default;

	// Iterators
	iterator begin() { return Iterator{ m_tree }; }
	iterator end() { return Iterator{}; }
	const_iterator begin() const { return ConstIterator{ m_tree }; }
	const_iterator end() const { return ConstIterator{}; }
	const_iterator cbegin() const { return ConstIterator{ m_tree }; }
	const_iterator cend() const { return ConstIterator{}; }

	// Capacity
	size_type size() { return std::distance(this->begin(), this->end()); }
	bool empty() { return std::distance(this->begin(), this->end()) == 0; }

	ASTNode *operator->() const
	{
		return m_tree.get();
	}

	AST tree_ref()
	{
		AST tmp{ m_tree };
		return tmp;
	}

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
} // namespace CoilCl

