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

#include <string>

class Module
{
	std::shared_ptr<Cry::Module::Interface> m_interface;

public:
	Module()
	{
	}

	std::string Name() const noexcept
	{
		m_interface->GetInfo().moduleName;
	}

	inline void Load() const noexcept
	{
		m_interface->OnLoad();
	}

	inline void Unload() const noexcept
	{
		m_interface->OnUnload();
	}
};

namespace ModuleLoader
{

// Load external module directly.
Module Load(const std::string& name);
//TOOD: load from directory, or file list

} // namespace ModuleLoader
