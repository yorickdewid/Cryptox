// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/Cry.h>
#include <Cry/Memory.h>
#include <Cry/Module.h>

#include <boost/dll.hpp>

#include <string>
#include <vector>
#include <array>

#define ANY_COMPONENT_ID 0

namespace dll = boost::dll;

static const std::array<std::string, 3> g_moduleExtensions = std::array<std::string, 3>{ ".dll", ".so", ".exe" };

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

template<typename ModuleClass = Interface>
class Module
{
	std::shared_ptr<ModuleClass> m_interface;

public:
	Module(std::shared_ptr<ModuleClass> interface)
		: m_interface{ interface }
	{
	}

	inline std::string Name() const noexcept
	{
		return m_interface->GetModuleInfo().moduleName;
	}

	inline std::string Author() const noexcept
	{
		return m_interface->GetModuleInfo().moduleAuthor;
	}

	inline std::string Copyright() const noexcept
	{
		return m_interface->GetModuleInfo().moduleCopyright;
	}

	inline void Load() const noexcept
	{
		m_interface->OnLoad();
	}

	inline void Unload() const noexcept
	{
		m_interface->OnUnload();
	}

	std::shared_ptr<ModuleClass> operator->()
	{
		return m_interface;
	}
};

// Load the file as external module, and return if failed.
template<typename ModuleClass = Cry::Module::Interface>
Cry::Module::Module<ModuleClass> LoadAsModule(boost::filesystem::path file, Cry::Module::ComponentId componentId = ANY_COMPONENT_ID, std::string symbol = EXPORT_SYMBOL_STR)
{
	if (!boost::filesystem::is_regular_file(file)) {
		throw LoaderException{ "file is not a module" };
	}

	bool isAllowed = std::any_of(g_moduleExtensions.cbegin(), g_moduleExtensions.cend(), [&file](const std::string& ext) {
		return file.extension() == ext;
	});
	if (!isAllowed) {
		throw LoaderException{ "file is not a module" };
	}

	dll::shared_library library(file, dll::load_mode::append_decorations);
	if (!library.has(symbol)) {
		throw LoaderException{ "module symbol not found" };
	}

	// Load the external module as a library.
	auto moduleInterface = MakeShared(dll::import<ModuleClass>(std::move(library), symbol));
	assert(moduleInterface);

	auto modInfo = moduleInterface->GetLibraryInfo();
	switch (modInfo.gatewayVersion)
	{
	case Cry::Module::GatewayVersion::VERSION_1:
		break;
	default:
		throw LoaderException{ "module is incompatible with this loader" };
	}

	// Test for component ID.
	if (componentId > ANY_COMPONENT_ID) {
		if (modInfo.componentId != componentId) {
			throw LoaderException{ "module is incompatible with this loader" };
		}
	}

	return moduleInterface;
}

// Load modules in the directory, skips every non compatible module.
template<typename ModuleClass>
std::vector<Module<ModuleClass>> Load(const std::string& name, ComponentId id)
{
	std::vector<Module<ModuleClass>> resultList;
	boost::filesystem::path path{ name };

	if (!boost::filesystem::is_directory(path)) {
		throw std::runtime_error{ "path is not a directory" };
	}

	for (boost::filesystem::directory_entry& dir : boost::filesystem::directory_iterator(path)) {
		try {
			resultList.emplace_back(LoadAsModule<ModuleClass>(dir, id));
		}
		catch (const Cry::Module::LoaderException&) {
			continue;
		}
	}

	return resultList;
}

// Load external module, throws if module cannot be loaded.
Cry::Module::Module<> LoadSingle(const std::string& name, Cry::Module::ComponentId id)
{
	return LoadAsModule(name, id);
}

// Load modules in the directory, skips every non compatible module.
template<typename ModuleClass>
std::vector<Module<ModuleClass>> Load(const std::string& name)
{
	return Load<ModuleClass>(name, ModuleClass::GetComponentId());
}

// Load external module, throws if module cannot be loaded.
template<typename ModuleClass>
Cry::Module::Module<ModuleClass> LoadSingle(const std::string& name)
{
	return LoadAsModule<ModuleClass>(name, ModuleClass::GetComponentId());
}

template<typename ModuleClass, typename Predicate>
void ForEach(std::vector<Module<ModuleClass>>& mods, Predicate pred)
{
	for (auto& mod : mods) {
		mod.Load();
		pred(mod);
		mod.Unload();
	}
}

} // namespace Module
} // namespace Cry
