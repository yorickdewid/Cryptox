// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Executable.h"

#include <iostream>

void someFunc()
{
	CryExe::Executable exe{ "testfile.cex" };
	
	if (exe.IsOpen()) {
		std::cout << "File is open" << std::endl;
	}

	exe.Close();
}