// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

//#include "Exportable.h"
#include "Executable.h"

//#include <string>

namespace CryExe
{

class COILCEXAPI Section
{
public:
	Section()
	{
		////
	}

	void PushContent()
	{

	}

	inline bool IsAllowedOnce() const { return true; }
};

} // namespace CryExe
