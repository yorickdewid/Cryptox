// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Cry/Except.h"

#include "ASTNode.h"

namespace CoilCl
{
namespace Emit
{
namespace Sequencer
{

class Interface
{
public:
	virtual void Execute(ASTNode *node) = 0;
};

class CASM : public Interface
{
public:
	virtual void Execute(ASTNode *node)
	{
		CRY_UNUSED(node);
		throw Cry::Except::NotImplementedException{ "CASM::Execute" };
	}
};

class AIIPX : public Interface
{
	std::function<void(uint8_t *data, size_t sz)> m_outputCallback;

public:
	AIIPX(std::function<void(uint8_t *data, size_t sz)> outputCallback)
		: m_outputCallback{ outputCallback }
	{
	}

	virtual void Execute(ASTNode *node);
};

} // namespace Sequencer
} // namespace Emit
} // namespace CoilCl
