// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Optimizer.h"

CoilCl::Optimizer::Optimizer(std::shared_ptr<CoilCl::Profile>& profile, AST::AST&& ast, ConditionTracker::Tracker& tracker)
	: Stage{ this, StageType::Type::SemanticAnalysis, tracker }
	, m_profile{ profile }
	, m_ast{ std::move(ast) }
{
}

CoilCl::Optimizer& CoilCl::Optimizer::CheckCompatibility()
{
	//TODO
	return (*this);
}

CoilCl::Optimizer& CoilCl::Optimizer::TrivialReduction()
{
	this->CompletePhase(ConditionTracker::OPTIMIZED);
	return (*this);
}

CoilCl::Optimizer& CoilCl::Optimizer::DeepInflation()
{
	return (*this);
}
