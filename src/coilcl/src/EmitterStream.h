// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Profile.h"
#include "Stage.h"

namespace CoilCl
{
namespace Emit
{
namespace Stream
{

class OutputStream
{
};

class Console : public OutputStream
{
};

class File : public OutputStream
{
};

class MemoryBlock : public OutputStream
{
};

} // namespace Stream
} // namespace Emit
} // namespace CoilCl
