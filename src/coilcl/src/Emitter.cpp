// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Emitter.h"

namespace CoilCl
{
namespace Emit
{

using namespace CryCC::Program;
using namespace CryCC::AST;

Emitter::Emitter(std::shared_ptr<Profile>& profile, AST&& ast, ConditionTracker::Tracker& tracker)
	: Stage{ this, StageType::Type::Emitter, tracker }
	, m_profile{ profile }
	, m_ast{ std::move(ast) }
{
}

Emitter& Emitter::CheckCompatibility()
{
	//TODO
	return (*this);
}

AST Emitter::Strategy(ModuleInterface::ModulePerm permission)
{
	switch (permission)
	{
	case ModuleInterface::ReadOnly:
		//TODO: mark ReadOnly
		return std::move(m_ast);

	case ModuleInterface::AppendData:
	case ModuleInterface::CopyOnWrite:
		return std::move(m_ast);

	case ModuleInterface::Substitute: {
		std::shared_ptr<ASTNode> k; //TODO
		m_ast.swap(k);
		return std::move(m_ast);
	}

	default:
		break;
	}

	return nullptr;
}

Emitter& Emitter::Process()
{
	// Reorder modules so that the most destructive modules are called first.
	std::sort(m_mods.begin(), m_mods.end(), [](ModulePair lhs, ModulePair rhs) {
		return lhs.first > rhs.first;
	});

	for (auto& mod : m_mods) {
		// Determine AST strategy.
		AST astWrapper = Strategy(mod.first);

		try {
			// Call module with tree.
			mod.second.Invoke(astWrapper);
		}
		catch (const ModuleException&) {
			throw; //TODO 
		}
	}

	return (*this);
}

Emitter& Emitter::RegisterModule(Module<Sequencer::Interface>&& module)
{
	// Request module permissions so we can register the module.
	const auto permInfo = module.RequestPermissionInfo();

	// Push module on list.
	m_mods.push_back({ permInfo, std::move(module) });
	return (*this);
}

} // namespace CoilCl
} // namespace Emit
