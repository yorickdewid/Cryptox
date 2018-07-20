// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <string>

#define EXPORT_SYMBOL _cry_mod_init
#define EXPORT_SYMBOL_STR "_cry_mod_init"
#define EXPORT_MODULE_CLASS(u) \
	extern "C" CRY_SYMBOL_EXPORT u EXPORT_SYMBOL; \
	u EXPORT_SYMBOL;

namespace Cry
{
namespace Module
{

enum GatewayVersion { VERSION_1 = 1 };

enum Properties
{
	PARALLEL = 1 << 0,    // 0000 0001
	AUTO_UNLOAD = 1 << 1, // 0000 0010
};

using VersionType = unsigned int;

struct Interface
{
	struct Info
	{
		VersionType apiVersion;
		GatewayVersion gatewayVersion;
		std::string moduleName;
		std::string moduleAuthor;
		std::string moduleCopyright;
	};

	// Get the module information, this method is always called regardless
	// of the contents of the module and must be implemented.
	virtual Info GetInfo() const noexcept = 0;

	// Called if the module is loaded.
	virtual void OnLoad() {}
	// Called before t the module is unloaded.
	virtual void OnUnload() {}

	virtual ~Interface() {}
};

} // namespace Module
} // namespace Cry
