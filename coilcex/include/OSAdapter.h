// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Exportable.h"

#include <string>

namespace CryExe
{

class COILCEXAPI OSAdapter
{
public:
	OSAdapter()
	{
	}

	virtual bool IsOpen() const { return true; }

protected:
	//void OpenWithMode(FileMode fm);
};

} // namespace CryExecutable
