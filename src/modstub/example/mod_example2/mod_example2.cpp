// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "MockInterface.h"

#include <boost/algorithm/string.hpp>

#define MOD_NAME         "mod_example2"
#define MOD_AUTHOR       "Blub Corp."
#define MOD_COPYRIGHT    "Copyright (c) 2018"

class ModExample2 : public MockInterface
{
public:
	// Return the information about this module. The information is used by the
	// module loader, exteral tools and publishing catalogue.
	Info GetModuleInfo() const noexcept
	{
		return MakeInfo(
			MOD_NAME,           // Module loader name.
			MOD_AUTHOR,         // Module author.
			MOD_COPYRIGHT       // Module copyright.
		);
	}

	// Must implement transform in this extension.
	void Transform(std::string& str)
	{
		boost::algorithm::to_upper(str);
	}
};

EXPORT_MODULE_CLASS(ModExample2);
