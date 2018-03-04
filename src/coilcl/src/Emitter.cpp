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

AST::AST Emitter::Strategy(ModuleInterface::ModulePerm permission)
{
	switch (permission)
	{
	case ModuleInterface::ReadOnly:
		break;
	case ModuleInterface::CopyOnWrite:
		break;
	case ModuleInterface::Substitute:
		break;
	default:
		break;
	}
	
	return nullptr; /* m_ast->;*/
}

Emitter& Emitter::Process()
{
	// Reorder modules so that the most destructive modules are called first
	std::sort(m_mods.begin(), m_mods.end(), [](ModulePair lhs, ModulePair rhs) {
		return lhs.first > rhs.first;
	});
	
	for (auto& mod : m_mods)
	{
		// Determine AST strategy
		auto& ast = Strategy(mod.first);

		// Call module with tree
		mod.second.Invoke(/*ast*/);
	}

	return (*this);
}


