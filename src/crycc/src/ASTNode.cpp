// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/AST/ASTNode.h>

#include <iostream>

using namespace CryCC::AST;

//
// Implement abstract class destructors.
//

Operator::~Operator() {}
Literal::~Literal() {}
Decl::~Decl() {}
Expr::~Expr() {}
Stmt::~Stmt() {}

ASTNode::ASTNode(CryCC::SourceLocation::value_type line, CryCC::SourceLocation::value_type col)
	: m_location{ line, col }
{
}

bool ASTNode::operator==(const ASTNode& other) const noexcept
{
	return (nodeId == other.nodeId)
		&& (m_location == other.m_location)
		&& (m_state.size() == other.m_state.size())
		&& (children.size() == other.children.size());
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

void ASTNode::SetLocation(CryCC::SourceLocation::value_type line, CryCC::SourceLocation::value_type col)
{
	m_location = { line, col };
}

void ASTNode::SetLocation(const std::pair<CryCC::SourceLocation::value_type, CryCC::SourceLocation::value_type>& location)
{
	m_location = { location };
}

void ASTNode::SetLocation(CryCC::SourceLocation&& location)
{
	std::swap(m_location, location);
}

CryCC::SourceLocation ASTNode::Location() const
{
	return m_location;
}

void ASTNode::Serialize(Serializable::VisitorInterface& pack)
{
	pack.SetId(UniqueObj::Id());
	pack << nodeId;
	pack << UniqueObj::Id();
	pack << m_location.Line();
	pack << m_location.Column();

	/*for (const auto& data : m_userData) {
	pack << data->Serialize();
	}*/
}

void ASTNode::Deserialize(Serializable::VisitorInterface& pack)
{
	AST::NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	SourceLocation::value_type line, col;
	pack >> UniqueObj::Id();
	pack >> line;
	pack >> col;

	m_location = { line, col };

	//TODO: deserialize user data
}
