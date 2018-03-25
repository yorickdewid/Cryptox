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

struct ASTFactory
{
	static std::shared_ptr<ASTNode> MakeNode(Serializable::Interface *visitor);
};

} // namespace AST
} // namespace CoilCl

