// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <iostream>
#include <iomanip>

namespace CoilCl
{
namespace Emit
{
namespace Stream
{

template<typename StreamType, typename... ArgTypes>
inline auto MakeStream(ArgTypes&&... args)
{
	return std::make_shared<StreamType>(std::forward<ArgTypes>(args)...);
}

// Stream input contract
class InputStream
{
	virtual void Read(std::vector<uint8_t>& vector)
	{
		CRY_UNUSED(vector);
	}

public:
	virtual void Read(uint8_t *vector, size_t sz) = 0;
};

// Stream output contract
class OutputStream
{
	virtual void Write(std::vector<uint8_t>& vector)
	{
		CRY_UNUSED(vector);
	}

public:
	virtual void Write(uint8_t *vector, size_t sz) = 0;
};

// Interact with the console. All output is written to the
// standard out console. Input is requested from the console.
// This stream structure is mainly used for debug and interactive
// shell environments.
class Console
	//: public InputStream
	: public OutputStream
{
public:
	// Write data stream to console output
	virtual void Write(uint8_t *vector, size_t sz)
	{
		for (size_t i = 0; i < sz; i++)
		{
			const int ch = static_cast<int>(static_cast<unsigned char>(vector[i]));
			std::cout << std::hex << std::setfill('0') << std::setw(2) << ch;
		}
		std::cout << std::flush;
	}
};

// Write or read data to file
class File
	//: public InputStream
	: public OutputStream
{
	//FUTURE: write content to disk in a frontend provided file.
public:
	virtual void Write(uint8_t *vector, size_t sz)
	{
		CRY_UNUSED(vector);
		CRY_UNUSED(sz);
	}
};

// Write or read data from memory slab
class MemoryBlock
	: public InputStream
	, public OutputStream
{
#define DEFAULT_MEMORY_BLOCK (2 << 9)

	void LocalFree()
	{
		if (m_doFree && m_block) {
			delete m_block;
			m_block = nullptr;
		}
	}

	// Reserve enough memory
	void ReserveMemory()
	{
		if (m_block->capacity() < DEFAULT_MEMORY_BLOCK / 2) {
			m_block->reserve(DEFAULT_MEMORY_BLOCK);
		}
	}

	void ResizeMemory()
	{
		m_block->reserve(m_block->capacity() + DEFAULT_MEMORY_BLOCK);
	}

public:
	using MemoryPool = std::vector<uint8_t>;

public:
	enum struct AccessControl
	{
		READ_WRITE,
		READ_ONLY,
	};

public:
	MemoryBlock(size_t capacity = 0)
		: m_block{ new MemoryPool{} }
		, m_doFree{ true }
	{
		if (capacity > 0) {
			m_block->reserve(capacity);
		}

		ReserveMemory();
	}

	// Write into the caller provided block
	explicit MemoryBlock(MemoryPool& block)
		: m_block{ &block }
		, m_doFree{ false }
	{
		ReserveMemory();
	}

	MemoryBlock(const MemoryBlock&) = delete;
	MemoryBlock(MemoryBlock&& other)
	{
		LocalFree();
		m_block = std::move(other.m_block);
		ReserveMemory();
	}

	~MemoryBlock()
	{
		LocalFree();
	}

	// Memory data size
	inline size_t Size() const noexcept { return m_block->size(); }
	// Shrink the capacity to size
	inline void Shrink() { m_block->shrink_to_fit(); }
	// Check if stream is depleted
	inline bool IsEoS() const noexcept { return m_readOffset == m_block->size(); }

	// Lock the memory block from buffer alteration
	void WriteDone()
	{
		Shrink();
		m_acl = AccessControl::READ_ONLY;
	}

	// Write data stream to memory block
	virtual void Write(uint8_t *vector, size_t sz)
	{
		if (m_acl != AccessControl::READ_WRITE) {
			CryImplExcept(); //TODO
		}

		if (sz < 1) { return; }
		m_block->insert(m_block->end(), vector, vector + sz);
		if (m_block->size() >= (m_block->capacity() * 0.1)) {
			ResizeMemory();
		}
	}

	// Read data stream from memory block
	virtual void Read(uint8_t *vector, size_t sz) override
	{
		if (sz < 1) { return; }
		if (IsEoS()) { return; }

		if (m_readOffset + sz > m_block->size()) {
			sz = m_block->size() - m_readOffset;
		}

		CRY_MEMCPY(vector, sz, m_block->data() + m_readOffset, sz);
		m_readOffset += sz;
	}

	// Copy entire memory block
	std::shared_ptr<MemoryBlock> DeepCopy()
	{
		auto ptr = std::make_shared<MemoryBlock>(this->m_block->size());
		ptr->m_block->assign(this->m_block->begin(), this->m_block->end());
		return ptr;
	}

private:
	bool m_doFree;
	size_t m_readOffset = 0;
	MemoryPool *m_block = nullptr;
	AccessControl m_acl{ AccessControl::READ_WRITE };
};

} // namespace Stream
} // namespace Emit
} // namespace CoilCl
