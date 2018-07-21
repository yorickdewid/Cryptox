// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <Cry/Cry.h>
#include <Cry/Module.h>

#define MOD_COMPONENT_ID 0
#define MOD_API_VERSION  2
#define MOD_NAME         "mod_example1"
#define MOD_AUTHOR       "Blub Corp."
#define MOD_COPYRIGHT    "Copyright (c) 2018"

class ModExample1 : public Cry::Module::Interface
{
	void *m_resource{ nullptr };

public:

	// Return the information about this module. The information is used by the
	// module loader, exteral tools and publishing catalogue.
	Info GetInfo() const noexcept
	{
		return { {
			MOD_GW_VERSION_1,   // Module communication version.
			MOD_PROP_NONE,      // Module properties.
			MOD_COMPONENT_ID }, // Module component.
			MOD_API_VERSION,    // Internal API version.
			MOD_NAME,           // Module loader name.
			MOD_AUTHOR,         // Module author.
			MOD_COPYRIGHT,      // Module copyright.
		};
	}

	// Called if the module is loaded.
	void OnLoad() override
	{
		m_resource = new char[1];
	}

	// Called before t the module is unloaded.
	void OnUnload()
	{
		delete[] m_resource;
		m_resource = nullptr;
	}
};

EXPORT_MODULE_CLASS(ModExample1);
