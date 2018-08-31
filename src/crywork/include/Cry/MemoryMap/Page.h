// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

namespace Cry
{
namespace MemoryMap
{

// This is used by `BasicMMap` to determine whether to create a read-only or a read-write memory mapping.
enum class AccessModeType
{
	READ,
	WRITE,
};

} // namespace Cry
} // namespace MemoryMap
