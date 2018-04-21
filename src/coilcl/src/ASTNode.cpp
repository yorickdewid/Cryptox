// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "ASTNode.h"

#include <iostream>

using namespace CoilCl::AST;

//
// Implement abstract class destructors
//

Operator::~Operator() {}
Literal::~Literal() {}
Decl::~Decl() {}
Expr::~Expr() {}
Stmt::~Stmt() {}

// Unique object counter initialization
int UniqueObj::_id = 100;

ASTNode::ASTNode(int _line, int _col)
	: line{ _line }
	, col{ _col }
{
}

// Print the AST tree to screen
void ASTNode::Print(int version, int level, bool last, std::vector<int> ignore) const
{
	if (level == 0) {
		std::cout << NodeName() << std::endl;
	}
	else {
		for (size_t i = 0; i < static_cast<size_t>(level) - 1; ++i) {
			if (std::find(ignore.begin(), ignore.end(), i) == ignore.end()) {
				std::cout << "| ";
			}
			else {
				std::cout << "  ";
			}
		}

		if (last) {
			ignore.push_back(level - 1);
			std::cout << "`-" << NodeName() << std::endl;
		}
		else {
			std::cout << "|-" << NodeName() << std::endl;
		}
	}

	// Traverse down the tree
	const auto traverse = [=](const std::vector<std::weak_ptr<ASTNode>>& cList)
	{
		for (const auto& weakChild : cList) {
			if (auto delegateChildren = weakChild.lock()) {
				delegateChildren->Print(version, level + 1, &weakChild == &cList.back(), ignore);
			}
		}
	};

	// If original version was requested, print node version zero
	if (version == 0 && m_state.HasAlteration()) {
		const auto& nodeVersion = m_state.front();
		traverse(nodeVersion->children);
		return;
	}

	traverse(children);
}

void ASTNode::SetLocation(int _line, int _col) const
{
	line = _line;
	col = _col;
}

void ASTNode::SetLocation(const std::pair<int, int>& loc) const
{
	line = loc.first;
	col = loc.second;
}

void ASTNode::SetLocation(Util::SourceLocation&& location)
{
	line = location.Line();
	col = location.Column();
}

std::pair<int, int> ASTNode::Location() const
{
	return { line, col };
}

void ASTNode::Serialize(Serializable::Interface& pack)
{
	pack.SetId(UniqueObj::Id());
	pack << nodeId;
	pack << UniqueObj::Id();
	pack << line;
	pack << col;

	/*for (const auto& data : m_userData) {
	pack << data->Serialize();
	}*/
}

void ASTNode::Deserialize(Serializable::Interface& pack)
{
	AST::NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	pack >> UniqueObj::Id();
	pack >> line;
	pack >> col;

	//TODO: deserialize user data
}
