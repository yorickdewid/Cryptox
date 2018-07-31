// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <Cry/Cry.h>
#include <Cry/MemoryManager.h>

using namespace Cry::MemoryManager;

ResourceType MultiPoolMemoryManager::HeapAllocate(size_t size)
{
	return std::find_if(m_pools.cbegin(), m_pools.cend(), [](const auto& pool) {
		CRY_UNUSED(pool);
		return true;
	})->get()->Allocate(size);
}

void MultiPoolMemoryManager::HeapFree(ResourceType ptr)
{
	return std::find_if(m_pools.cbegin(), m_pools.cend(), [](const auto& pool) {
		CRY_UNUSED(pool);
		return true;
	})->get()->Deallocate(ptr);
}

size_t MultiPoolMemoryManager::HeapTotalSize() const noexcept
{
	size_t result = 0;
	for (const auto& pool : m_pools) {
		result += pool->TotalSize();
	}
	return result;
}

size_t MultiPoolMemoryManager::HeapFreeSize() const noexcept
{
	size_t result = 0;
	for (const auto& pool : m_pools) {
		result += pool->FreeSize();
	}
	return result;
}

size_t MultiPoolMemoryManager::HeapUsedSize() const noexcept
{
	size_t result = 0;
	for (const auto& pool : m_pools) {
		result += pool->UsedSize();
	}
	return result;
}
