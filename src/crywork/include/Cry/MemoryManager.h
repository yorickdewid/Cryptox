// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Cry.h"
#include "MemoryManager/Interface.h"

#include <list>
#include <memory>
#include <algorithm>
#include <bitset>
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

class BackedPool : public MemoryPoolInterface
{

public:
	BackedPool() = default;
	BackedPool(BackedPool&&)
	{
	}

	virtual ~BackedPool()
	{
	}

	// Request memory block.
	void *Allocate(size_t size)
	{
		return malloc(size);
	}

	// Free allocated memory.
	void Deallocate(void *ptr)
	{
		if (!ptr) { return; }
		free(ptr);
	}

	size_t TotalSize() const noexcept
	{
		return 0;
	}

	size_t FreeSize() const noexcept
	{
		return 0;
	}

	size_t UsedSize() const noexcept
	{
		return 0;
	}
};

template<size_t AllocSize>
class ContiguousPool : public MemoryPoolInterface
{
	constexpr static size_t chunkSize = 128;

	static_assert(AllocSize > chunkSize, "must be at least chunk size");

	void *m_block{ nullptr };
	std::bitset<AllocSize / chunkSize> m_free;

protected:
	virtual void FreePool()
	{
		if (!m_block) { return; }
		free(m_block);
		m_block = nullptr;
	}

	// Block preamble.
	struct MemoryPointer
	{
		size_t blocks;
	};

	virtual void *AllocateNewChunk(size_t size)
	{
		assert(m_block);
		size_t blocks = ((size + sizeof(MemoryPointer)) / chunkSize) + 1;

		size_t contiguousBlocks = blocks;
		for (size_t i = 0; i < m_free.size(); ++i) {
			if (!m_free[i]) {
				for (size_t j = i; j < contiguousBlocks + i; ++j) {
					if (j >= m_free.size()) {
						throw OutOfMemoryException{ size };
					}
					if (m_free[j]) {
						goto skip_block;
					}
				}
				for (size_t j = i; j < contiguousBlocks + i; ++j) {
					m_free.set(j);
				}

				MemoryPointer *memPtr = (MemoryPointer*)(((uint8_t*)m_block) + (i * chunkSize));
				memPtr->blocks = blocks;
				return memPtr + sizeof(MemoryPointer);
			skip_block: {}
			}
		}

		throw OutOfMemoryException{ size };
	}

	virtual void ReturnChunk(void *ptr)
	{
		assert(m_block);
		MemoryPointer *memPtr = (MemoryPointer *)ptr - sizeof(MemoryPointer);

		size_t i = (((uint8_t*)memPtr) - ((uint8_t*)m_block)) / chunkSize;
		for (size_t j = i; j < memPtr->blocks + i; ++j) {
			m_free.reset(j);
		}
	}

public:
	ContiguousPool()
		: m_block{ malloc(AllocSize) }
		, m_free{ 0 }
	{
	}

	ContiguousPool(ContiguousPool&& other)
		: m_block{ other.m_block }
	{
		other.m_block = nullptr;
	}

	virtual ~ContiguousPool()
	{
		FreePool();
	}

	// Request memory block.
	void *Allocate(size_t size)
	{
		return AllocateNewChunk(size);
	}

	// Free allocated memory.
	void Deallocate(void *ptr)
	{
		ReturnChunk(ptr);
	}

	size_t TotalSize() const noexcept
	{
		return m_free.size() * chunkSize;
	}

	size_t FreeSize() const noexcept
	{
		return (m_free.size() - m_free.count()) * chunkSize;
	}

	size_t UsedSize() const noexcept
	{
		return m_free.count() * chunkSize;
	}

	void Clear() noexcept override
	{
		FreePool();
	}
};

// Ensure deallocated memory is whiped, extends any memory pool.
template<typename PoolType>
class SecurePoolImpl : public PoolType
{
	static_assert(std::is_base_of<MemoryPoolInterface, PoolType>::value, "must implement MemoryPoolInterface");

public:
	// Free allocated memory and clear.
	virtual void Deallocate(void *ptr) override
	{
		// TODO: nullify

		PoolType::Deallocate(ptr);
	}

	virtual ~SecurePoolImpl()
	{
		//CRY_MEMZERO(m_block, this->TotalSize());
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
