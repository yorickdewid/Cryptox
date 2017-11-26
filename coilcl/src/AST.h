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

class AST
{
	using _ValTy = ASTNode;

public: // Member types
	using value_type = _ValTy;
	using reference = _ValTy&;
	using const_reference = const _ValTy&;
	using pointer = _ValTy*;
	using const_pointer = const _ValTy*;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

public:
	class Iterator
	{
		std::shared_ptr<ASTNode> cNode;

	private:
		using _MyTy = Iterator;

	public:
		using value_type = _ValTy;
		using reference = _ValTy&;
		using pointer = _ValTy*;
		using difference_type = std::ptrdiff_t;

		Iterator() : cNode{ nullptr } {}
		Iterator(std::shared_ptr<ASTNode>& node) : cNode{ node } {}
		Iterator(const Iterator&) = default;

		_MyTy& operator++()
		{
			this->operator++(1);
			return (*this);
		}

		_MyTy& operator++(int adv)
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
						return (*this);
					}
					else {
						auto weakNeighbour = selfListItem + 1;
						if (auto neighbour = weakNeighbour->lock()) {
							cNode = neighbour;
						}
					}
				}
				else {
					//cNode = nullptr;
				}
			}
			else {
				auto firstWeakChild = cNode->At(0);
				if (auto firstChild = firstWeakChild.lock()) {
					cNode = firstChild;
				}
				else {
					//cNode = nullptr;
				}
			}

			return (*this);
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

	/*class ConstIterator
	{
	public:
		ConstIterator(const int*)
		{
		}
	};*/

public:
	using iterator = Iterator;
	//using const_iterator = ConstIterator;

	AST(std::shared_ptr<ASTNode>&& tree)
		: m_tree{ std::move(tree) }
	{
	}

	// Iterators
	iterator begin() { return Iterator{ m_tree }; }
	iterator end() { return Iterator{}; }

	// 

	/*const_iterator begin() const
	{
		return ConstIterator(a);
	}

	const_iterator end() const
	{
		return ConstIterator(a);
	}*/

private:
	std::shared_ptr<ASTNode> m_tree;
};

} // namespace AST
} // namespace CoilCl

