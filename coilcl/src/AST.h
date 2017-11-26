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
public:
	class Iterator
	{
		std::shared_ptr<ASTNode> cNode;

	public:
		Iterator(std::shared_ptr<ASTNode>& node) : cNode{ node } {}

		Iterator operator++()
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

					if (selfListItem == parentChildren.end()) {
						//nope
						printf("lolz");
					}

					auto weakNeighbour = selfListItem + 1;
					if (auto neighbour = weakNeighbour->lock()) {
						cNode = neighbour;
					}
				}
				else {
					// Broken iterator ? Root ?
				}
			}
			else {
				auto firstWeakChild = cNode->At(0);
				if (auto firstChild = firstWeakChild.lock()) {
					cNode = firstChild;
				}
				else {
					///
				}
			}

			return *this;
		}

		ASTNode& operator*()
		{
			return *(cNode.get());
		}

		ASTNode& operator->()
		{
			return *(cNode.get());
		}

		bool operator!=(Iterator const &other) const
		{
			return cNode != other.cNode;
		}
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

	iterator begin()
	{
		return Iterator(m_tree);
	}

	iterator end()
	{
		std::shared_ptr<ASTNode> node = m_tree;

		for (;;) {
			if (node->ChildrenCount() == 0) {
				return Iterator(node);
			}
			else {
				if (auto lastChild = node->Children().back().lock()) {
					node = lastChild;
				}
				else {
					throw std::exception{};
				}
			}
		}

		throw std::exception{};
	}

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

