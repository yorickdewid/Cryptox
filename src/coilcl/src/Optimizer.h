// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

// Local includes.
#include "Profile.h"

// Project includes.
#include <CryCC/Program.h>
#include <CryCC/AST.h>

namespace CoilCl
{

//TODO:
// - Remove unused code
// - Calculate constants

class Optimizer : public CryCC::Program::Stage<Optimizer>
{
public:
	Optimizer(std::shared_ptr<CoilCl::Profile>& profile, CryCC::AST::AST&& ast, CryCC::Program::ConditionTracker::Tracker&);

	std::string Name() const noexcept { return "Optimizer"; }

	Optimizer& CheckCompatibility();
	Optimizer& TrivialReduction();
	Optimizer& DeepInflation();

private:
	CryCC::AST::AST m_ast;
	std::shared_ptr<CoilCl::Profile> m_profile;
};

} // namespace CoilCl
