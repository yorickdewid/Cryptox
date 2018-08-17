// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/Cry.h>

#include <string>

#define EXPORT_SYMBOL _cry_mod_init
#define EXPORT_SYMBOL_STR "_cry_mod_init"
#define EXPORT_MODULE_CLASS(u) \
	extern "C" CRY_SYMBOL_EXPORT u EXPORT_SYMBOL; \
	u EXPORT_SYMBOL;

#define MOD_PROP_NONE         0 << 0  // 0000 0000
#define MOD_PROP_PARALLEL     1 << 0  // 0000 0001
#define MOD_PROP_AUTO_UNLOAD  1 << 1  // 0000 0010

#define MOD_GW_VERSION_1   Cry::Module::GatewayVersion::VERSION_1
#define MOD_GW_DEFAULT     MOD_GW_VERSION_1

namespace Cry
{
namespace Module
{

enum GatewayVersion { VERSION_1 = 1 };

using VersionType = unsigned int;
using ComponentId = unsigned int;

struct Interface
{
	struct BasicModule
	{
		GatewayVersion gatewayVersion;
		ComponentId componentId;
		unsigned int properties;
	};

	struct Info
	{
		Info(VersionType apiVersion
			, std::string moduleName
			, std::string moduleAuthor = {}
			, std::string moduleCopyright = {})
			: apiVersion{ apiVersion }
			, moduleName{ moduleName }
			, moduleAuthor{ moduleAuthor }
			, moduleCopyright{ moduleCopyright }
		{
		}

		VersionType apiVersion;
		std::string moduleName;
		std::string moduleAuthor;
		std::string moduleCopyright;
	};

	Interface(BasicModule info)
		: m_modBasics{ info }
	{
	}

	// Get the module information, this method is always called regardless
	// of the contents of the module and must be implemented.
	virtual Info GetModuleInfo() const noexcept = 0;

	// Called if the module is loaded.
	virtual void OnLoad() {}
	// Called before t the module is unloaded.
	virtual void OnUnload() {}

	inline BasicModule GetLibraryInfo() const noexcept
	{
		return m_modBasics;
	}

	static BasicModule MakeDefaultModule(ComponentId id)
	{
		return { MOD_GW_DEFAULT, id, MOD_PROP_NONE };
	}

	virtual ~Interface() {}

private:
	BasicModule m_modBasics;
};

} // namespace Module
} // namespace Cry
