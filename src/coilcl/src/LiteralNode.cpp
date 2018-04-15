// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "ASTNode.h"

#include <boost/format.hpp>

void Literal::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	if (HasReturnType()) {
		pack << true;
		std::vector<uint8_t> buffer;
		AST::TypeFacade::Serialize(ReturnType(), buffer);
		pack << buffer;
	}
	else {
		pack << false;
	}

	ASTNode::Serialize(pack);
}

void Literal::Deserialize(Serializable::Interface& pack)
{
	AST::NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	bool hasReturn = false;
	pack >> hasReturn;
	if (hasReturn) {
		std::vector<uint8_t> buffer;
		pack >> buffer;
		AST::TypeFacade::Deserialize(UpdateReturnType(), buffer);
	}

	ASTNode::Deserialize(pack);
}
