// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/AST/AST.h>

namespace CryCC
{
namespace AST
{

void ForwardItemTree::ForwardInternalTree(std::shared_ptr<ASTNode>& node)
{
	// No children in this node, work upwards and sideways
	if (node->ChildrenCount() == 0) {
	redo:
		auto weakParent = node->Parent();
		if (auto parent = weakParent.lock()) {
			auto parentChildren = parent->Children();

			auto selfListItem = std::find_if(parentChildren.begin(), parentChildren.end(), [=](std::weak_ptr<ASTNode>& wPtr)
			{
				return wPtr.lock() == node;
			});

			// Parent cannot find this child, pass control back to parent
			if (selfListItem == parentChildren.end()) {
				node = parent;
				goto redo;
			}

			// Reached end of parent children list, move up
			if (selfListItem + 1 == parentChildren.end()) {
				node = parent;
				goto redo;
			}
			// Pick right neighbor as next node
			else {
				auto weakNeighbor = selfListItem + 1;
				if (auto neighbor = weakNeighbor->lock()) {
					node = neighbor;
				}
			}
		}
		else {
			node = nullptr;
		}
	}
	// Node has children, descent to outer left
	else {
		auto firstWeakChild = node->At(0);
		if (auto firstChild = firstWeakChild.lock()) {
			node = firstChild;
		}
		else {
			node = nullptr;
		}
	}
}

} // namespace CryCC
} // namespace AST
