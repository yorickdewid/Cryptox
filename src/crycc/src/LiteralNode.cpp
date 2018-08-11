// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/AST/ASTNode.h>

#include <boost/format.hpp>

namespace CryCC
{
namespace AST
{

void Literal::Serialize(Serializable::VisitorInterface& pack)
{
	pack << nodeId;

	if (HasReturnType()) {
		pack << true;
		Cry::ByteArray buffer;
		Typedef::TypeFacade::Serialize(int{}, ReturnType(), buffer);
		pack << buffer;
	}
	else {
		pack << false;
	}

	ASTNode::Serialize(pack);
}

void Literal::Deserialize(Serializable::VisitorInterface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	bool hasReturn = false;
	pack >> hasReturn;
	if (hasReturn) {
		Cry::ByteArray buffer;
		pack >> buffer;
		Typedef::TypeFacade::Deserialize(int{}, UpdateReturnType(), buffer);
	}

	ASTNode::Deserialize(pack);
}

} // namespace CryCC
} // namespace AST
