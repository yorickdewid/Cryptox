// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/Cry.h>
#include <Cry/MemoryManager/Interface.h>
#include <Cry/MemoryManager/Pool.h>

#include <list>
#include <memory>
#include <algorithm>
#include <cassert>

#define DEFAULT_POOL_SZ 4096 << 9 // 2MB

namespace Cry
{
namespace MemoryManager
{

using ResourceType = void*;

struct OutOfMemoryException : public std::exception
{
	size_t m_allocatedSize;

public:
	OutOfMemoryException(size_t size)
		: m_allocatedSize{ size }
	{
	}

	virtual char const *what() const
	{
		return ""; //TODO
	}
};

// Memory manager delegating block allocation to memory pools.
class MultiPoolMemoryManager : public MemoryManagerInterface
{
	std::list<std::unique_ptr<MemoryPoolInterface>> m_pools;

public:
	MultiPoolMemoryManager() = default;

	template<typename MemoryPoolType>
	MultiPoolMemoryManager(MemoryPoolType&& pool)
	{
		static_assert(std::is_move_constructible<MemoryPoolType>::value, "must be moveable");

		m_pools.emplace_back(std::move(std::make_unique<MemoryPoolType>(std::move(pool))));
	}

	// Return number of pools in use.
	inline size_t PoolCount() const noexcept { return m_pools.size(); }

	// Reqest memory block from heap.
	ResourceType HeapAllocate(size_t size);
	// Free heap allocated memory.
	void HeapFree(ResourceType ptr);

	size_t HeapTotalSize() const noexcept;
	size_t HeapFreeSize() const noexcept;
	size_t HeapUsedSize() const noexcept;
};

// Default memory manager with predefined pool.
class MemoryManagerImpl : public MultiPoolMemoryManager
{
public:
	MemoryManagerImpl()
		: MultiPoolMemoryManager{ ContiguousPool<DEFAULT_POOL_SZ>{} }
	{
	}
};

using SecurePool = SecurePoolImpl<ContiguousPool<DEFAULT_POOL_SZ>>;
using MemoryManager = MemoryManagerImpl;

} // namespace MemoryManager
} // namespace Cry
