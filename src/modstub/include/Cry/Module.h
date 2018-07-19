// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <string>

namespace Cry
{
namespace Module
{

enum GatewayVersion { VERSION_1 = 1 };

struct Interface
{
	struct Info
	{
		unsigned int apiVersion;
		GatewayVersion gatewayVersion;
		//std::string fileName;
		//std::string className;
		std::string moduleName;
		std::string moduleAuthor;
		std::string moduleCopyright;
	};

	// Get the module information, this method is always called regardless
	// of the contents of the module and must be implemented.
	virtual Info GetInfo() const = 0;

	// Called if the module is loaded.
	virtual void OnLoad() {}
	// Called before t the module is unloaded.
	virtual void OnUnload() {}

	virtual ~Interface() {}
};

} // namespace Module
} // namespace Cry
