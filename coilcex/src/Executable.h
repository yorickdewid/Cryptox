// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Image.h"

#include <string>

namespace CryExe
{

class Executable : public Image
{
public:
	Executable(const std::string& path, FileMode fm = FileMode::FM_OPEN)
		: Image{ path, fm }
	{
	}

private:
	void ValidateImageFormat();
};

} // namespace CryExecutable