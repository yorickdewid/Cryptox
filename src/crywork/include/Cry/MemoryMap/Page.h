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

// This is used by `basic_mmap` to determine whether to create a read-only or
// a read-write memory mapping.
enum class access_mode
{
	read,
	write
};

// Determines the operating system's page allocation granularity.
//
// On the first call to this function, it invokes the operating system specific syscall
// to determine the page size, caches the value, and returns it. Any subsequent call to
// this function serves the cached value, so no further syscalls are made.
inline size_t page_size()
{
	static const size_t page_size = []
	{
#ifdef _WIN32
		SYSTEM_INFO SystemInfo;
		GetSystemInfo(&SystemInfo);
		return SystemInfo.dwAllocationGranularity;
#else
		return sysconf(_SC_PAGE_SIZE);
#endif
	}();
	return page_size;
}

// Alligns `offset` to the operating's system page size such that it subtracts the
// difference until the nearest page boundary before `offset`, or does nothing if
// `offset` is already page aligned.
inline size_t make_offset_page_aligned(size_t offset) noexcept
{
	const size_t page_size_ = page_size();
	// Use integer division to round down to the nearest page alignment.
	return offset / page_size_ * page_size_;
}

} // namespace Cry
} // namespace MemoryMap
