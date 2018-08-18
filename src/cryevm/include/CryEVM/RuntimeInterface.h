// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/Cry.h>
#include <Cry/Module.h>

#include <list>

#define MOD_COMPONENT_ID 19
#define MOD_API_VERSION  1

// Module interface for runtime extensions.
class RuntimeInterface : public Cry::Module::Interface
{
public:
	static Cry::Module::ComponentId GetComponentId()
	{
		return MOD_COMPONENT_ID;
	}

public:
	RuntimeInterface()
		: Cry::Module::Interface{ MakeDefaultModule(GetComponentId()) }
	{
	}

	// Load symbol index, if any.
	virtual void LoadSymbolIndex(std::list<EVM::ExternalMethod>&) {}

protected:
	Info MakeInfo(std::string moduleName, std::string moduleAuthor = {}, std::string moduleCopyright = {}) const noexcept
	{
		return {
			MOD_API_VERSION,
			moduleName,
			moduleAuthor,
			moduleCopyright
		};
	}
};
