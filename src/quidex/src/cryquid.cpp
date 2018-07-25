// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <ExternalMethod.h>
#include <ExternalValue.h>
#include <RuntimeInterface.h>

#define MOD_NAME         "quidex"
#define MOD_AUTHOR       "Blub Corp."
#define MOD_COPYRIGHT    "Copyright (c) 2018"

class QuidWrapper : public RuntimeInterface
{
public:
	// Return the information about this module.
	Info GetModuleInfo() const noexcept
	{
		return MakeInfo(
			MOD_NAME,           // Module loader name.
			MOD_AUTHOR,         // Module author.
			MOD_COPYRIGHT       // Module copyright.
		);
	}

	// Load functions from this module.
	void LoadSymbolIndex(std::list<EVM::ExternalMethod>&)  override
	{
		// list += IO::RegisterFunctions();
	}
};

EXPORT_MODULE_CLASS(QuidWrapper);
