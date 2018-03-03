// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "AST.h"
#include "Emitter.h"

using namespace CoilCl::Emit;

Emitter::Emitter(std::shared_ptr<Profile>& profile, AST::AST&& ast)
	: Stage{ this, StageType::Type::Emitter }
	, m_profile{ profile }
	, m_ast{ std::move(ast) }
{
}

Emitter& Emitter::CheckCompatibility()
{
	//TODO
	return (*this);
}

Emitter& Emitter::DumpSink()
{
	return (*this);
}
