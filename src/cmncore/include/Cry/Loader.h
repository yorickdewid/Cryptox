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
#include <vector>

namespace Cry
{
namespace Module
{

class LoaderException : public std::runtime_error
{
public:
	LoaderException(const std::string& msg)
		: std::runtime_error{ msg }
	{
	}

private:

};

class Module
{
	std::shared_ptr<Cry::Module::Interface> m_interface;

public:
	Module(std::shared_ptr<Cry::Module::Interface> interface)
		:m_interface{ interface }
	{
	}

	inline std::string Name() const noexcept
	{
		return m_interface->GetInfo().moduleName;
	}

	inline std::string Author() const noexcept
	{
		return m_interface->GetInfo().moduleAuthor;
	}

	inline std::string Copyright() const noexcept
	{
		return m_interface->GetInfo().moduleCopyright;
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

// Load modules in the directory, skips every non compatible module.
std::vector<Cry::Module::Module> Load(const std::string& name);
// Load external module, throws if module cannot be loaded.
Cry::Module::Module LoadSingle(const std::string& name);

} // namespace Module
} // namespace Cry
