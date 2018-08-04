// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

// Project includes.
#include <CryCC/AST.h>

// Framework includes.
#include <Cry/Except.h>

namespace CoilCl
{
namespace Emit
{
namespace Sequencer
{

using namespace CryCC::AST;

class Interface
{
public:
	// Execute the sequencer.
	virtual void Execute(AST) = 0;
};

class CASM : public Interface
{
public:
	virtual void Execute(AST)
	{
		throw Cry::Except::NotImplementedException{ "CASM::Execute" };
	}
};

class AIIPX : public Interface
{
	using IOCallback = std::function<void(uint8_t *data, size_t sz)>;

	// Input/Output stream callbacks.
	IOCallback m_outputCallback;
	IOCallback m_inputCallback;

public:
	AIIPX(IOCallback outputCallback, IOCallback inputCallback)
		: m_outputCallback{ outputCallback }
		, m_inputCallback{ inputCallback }
	{
	}

	// Implement interface.
	virtual void Execute(AST tree)
	{
		PackAST(tree);
	}

	// Convert tree into output stream.
	void PackAST(AST);
	// Convert input stream into tree.
	void UnpackAST(AST&);
};

} // namespace Sequencer
} // namespace Emit
} // namespace CoilCl
