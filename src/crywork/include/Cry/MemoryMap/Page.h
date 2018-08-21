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

// Determines the operating system's page allocation granularity.
//
// On the first call to this function, it invokes the operating system specific syscall
// to determine the page size, caches the value, and returns it. Any subsequent call to
// this function serves the cached value, so no further syscalls are made.
//size_t PageSize();

// Alligns `offset` to the operating's system page size such that it subtracts the
// difference until the nearest page boundary before `offset`, or does nothing if
// `offset` is already page aligned.
//size_t MakeOffsetPageAligned(size_t offset) noexcept;

} // namespace Cry
} // namespace MemoryMap
