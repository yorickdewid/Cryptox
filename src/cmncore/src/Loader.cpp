// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <Cry/Loader.h>

#include <boost/dll.hpp>

#include <iostream>
#include <array>

#define NONE_COMPONENT_ID 0

namespace dll = boost::dll;

static const std::array<std::string, 2> g_moduleExtensions = std::array<std::string, 2>{ ".dll", ".so" };

namespace
{

template<typename SharedPointer>
class Holder
{
	SharedPointer m_ptr;

public:
	Holder(const SharedPointer& p) : m_ptr(p) {}
	Holder(const Holder& other) = default;
	Holder(Holder&& other) = default;

	SharedPointer Get() const noexcept { return m_ptr; }

	void operator () (...) { m_ptr.reset(); }
};

} // namespace

namespace Cry
{

// Convert boost shared pointer into std shared pointer.
template<typename Type>
std::shared_ptr<Type> MakeShared(const boost::shared_ptr<Type>& p)
{
	using HolderType = Holder<std::shared_ptr<Type>>;
	using WrapperType = Holder<boost::shared_ptr<Type>>;
	if (HolderType *holder = boost::get_deleter<HolderType, Type>(p)) {
		return holder->Get();
	}

	return { p.get(), WrapperType{ p } };
}

} // namespace Cry

// Load the file as external module, and return if failed.
Cry::Module::Module LoadAsModule(boost::filesystem::path file, unsigned int componentId = NONE_COMPONENT_ID, std::string symbol = EXPORT_SYMBOL_STR)
{
	if (!boost::filesystem::is_regular_file(file)) {
		throw Cry::Module::LoaderException{ "file is not a module" };
	}

	bool isAllowed = std::any_of(g_moduleExtensions.cbegin(), g_moduleExtensions.cend(), [&file](const std::string& ext) {
		return file.extension() == ext;
	});
	if (!isAllowed) {
		throw Cry::Module::LoaderException{ "file is not a module" };
	}

	dll::shared_library library(file, dll::load_mode::append_decorations);
	if (!library.has(symbol)) {
		throw Cry::Module::LoaderException{ "module symbol not found" };
	}

	// Load the external module as a library.
	auto moduleInterface = Cry::MakeShared(dll::import<Cry::Module::Interface>(std::move(library), symbol));
	assert(moduleInterface);

	auto modInfo = moduleInterface->GetInfo();
	switch (modInfo.gatewayVersion)
	{
	case Cry::Module::GatewayVersion::VERSION_1:
		break;
	default:
		throw Cry::Module::LoaderException{ "module is incompatible with this loader" };
	}

	// Test for component ID.
	if (componentId > NONE_COMPONENT_ID) {
		if (!modInfo.componentId == componentId) {
			throw Cry::Module::LoaderException{ "module is incompatible with this loader" };
		}
	}

	return moduleInterface;
}

std::vector<Cry::Module::Module> Cry::Module::Load(const std::string& name)
{
	std::vector<Cry::Module::Module> resultList;
	boost::filesystem::path path{ name };

	if (!boost::filesystem::is_directory(path)) {
		throw std::runtime_error{ "path is not a directory" };
	}

	for (boost::filesystem::directory_entry& dir : boost::filesystem::directory_iterator(path)) {
		try {
			resultList.emplace_back(std::move(LoadAsModule(dir)));
		}
		catch (const Cry::Module::LoaderException&) {
			continue;
		}
	}

	return resultList;
}

Cry::Module::Module Cry::Module::LoadSingle(const std::string& name)
{
	return LoadAsModule(name);
}
