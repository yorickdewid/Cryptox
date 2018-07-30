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
namespace MemoryManager
{

using ResourceType = void*;

struct MemoryPoolInterface //TODO: NoCopy
{
	virtual ~MemoryPoolInterface() {}

	//
	// Memory block operations.
	//

	// Reqest memory block.
	virtual void *Allocate(size_t) = 0;
	// Free allocated memory.
	virtual void Deallocate(void *) = 0;

	//
	// Memory statistics.
	//

	virtual size_t TotalSize() const noexcept = 0;
	virtual size_t FreeSize() const noexcept = 0;
	virtual size_t UsedSize() const noexcept = 0;

	// Clear all allocated memory.
	virtual void Clear() noexcept {}
};

struct MemoryManagerInterface
{
	//
	// Memory block operations.
	//

	// Reqest memory block from heap.
	virtual ResourceType HeapAllocate(size_t) = 0;
	// Free heap allocated memory.
	virtual void HeapFree(ResourceType) = 0;

	//
	// Memory statistics.
	//

	virtual size_t HeapTotalSize() const noexcept = 0;
	virtual size_t HeapFreeSize() const noexcept = 0;
	virtual size_t HeapUsedSize() const noexcept = 0;
};

} // namespace MemoryManager
} // namespace Cry
