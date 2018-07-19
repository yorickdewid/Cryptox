// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

namespace EVM
{

struct MemoryManagerInterface
{
	//
	virtual void Deallocate() = 0;
	//
	virtual void Allocate() = 0;
};

class BlockBack : public MemoryManagerInterface
{
public:

};

} // namespace EVM
