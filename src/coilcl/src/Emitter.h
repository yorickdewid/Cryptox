// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Profile.h"
#include "Stage.h"
#include "EmitterStream.h"

namespace CoilCl
{
namespace Emit
{

enum Sequence
{
	CASM,
	AIIPX,
};

template<Sequence _Seq>
class Sink
{
public:
	template<typename _Ty>
	void AddStream(std::shared_ptr<_Ty>& ptr) {}
};

class Emitter : public Stage<Emitter>
{
public:
	Emitter(std::shared_ptr<CoilCl::Profile>& profile, AST::AST&& ast);

	std::string Name() const { return "Emitter"; }

	Emitter& CheckCompatibility();
	Emitter& DumpSink();

	template<typename _Ty>
	Emitter& AddSink(_Ty ptr)
	{
		return *this;
	}


private:
	AST::AST m_ast;
	std::shared_ptr<CoilCl::Profile> m_profile;
};

} // namespace Emit
} // namespace CoilCl
