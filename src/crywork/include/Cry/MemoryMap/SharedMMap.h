// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/MemoryMap/MMap.h>

#include <system_error>
#include <memory>

namespace Cry
{
namespace MemoryMap
{

// Exposes (nearly) the same interface as 'BasicMMap', but endowes it with
// 'std::shared_ptr' semantics.
//
// This is not the default behaviour of 'BasicMMap' to avoid allocating on
// the heap if shared semantics are not required.
template<access_mode AccessMode, typename ByteType>
class BasicSharedMMap
{
	using self_type = BasicMMap<AccessMode, ByteType>;
	std::shared_ptr<self_type> pimpl_;

public:
	using value_type = typename self_type::value_type;
	using size_type = typename self_type::size_type;
	using reference = typename self_type::reference;
	using const_reference = typename self_type::const_reference;
	using pointer = typename self_type::pointer;
	using const_pointer = typename self_type::const_pointer;
	using difference_type = typename self_type::difference_type;
	using iterator = typename self_type::iterator;
	using const_iterator = typename self_type::const_iterator;
	using reverse_iterator = typename self_type::reverse_iterator;
	using const_reverse_iterator = typename self_type::const_reverse_iterator;
	using iterator_category = typename self_type::iterator_category;
	using handle_type = typename self_type::handle_type;
	using mmap_type = self_type;

	BasicSharedMMap() = default;
	BasicSharedMMap(const BasicSharedMMap&) = default;
	BasicSharedMMap& operator=(const BasicSharedMMap&) = default;
	BasicSharedMMap(BasicSharedMMap&&) = default;
	BasicSharedMMap& operator=(BasicSharedMMap&&) = default;

	// Takes ownership of an existing mmap object.
	BasicSharedMMap(mmap_type&& mmap)
		: pimpl_(std::make_shared<mmap_type>(std::move(mmap)))
	{
	}

	// Takes ownership of an existing mmap object.
	BasicSharedMMap& operator=(mmap_type&& mmap)
	{
		pimpl_ = std::make_shared<mmap_type>(std::move(mmap));
		return *this;
	}

	// Initializes this object with an already established shared mmap.
	BasicSharedMMap(std::shared_ptr<mmap_type> mmap)
		: pimpl_(std::move(mmap))
	{
	}

	// Initializes this object with an already established shared mmap.
	BasicSharedMMap& operator=(std::shared_ptr<mmap_type> mmap)
	{
		pimpl_ = std::move(mmap);
		return *this;
	}

	// The same as invoking the 'map' function, except any error that may occur while
	// establishing the mapping is thrown.
	template<typename String>
	BasicSharedMMap(const String& path, const size_type offset, const size_type length)
	{
		std::error_code error;
		map(path, offset, length, error);
		if (error) { throw error; }
	}

	// The same as invoking the 'map' function, except any error that may occur while
	// establishing the mapping is thrown.
	BasicSharedMMap(const handle_type handle, const size_type offset, const size_type length)
	{
		std::error_code error;
		map(handle, offset, length, error);
		if (error) { throw error; }
	}

	~BasicSharedMMap() = default;

	// Returns the underlying 'std::shared_ptr' instance that holds the mmap.
	std::shared_ptr<mmap_type> get_shared_ptr() { return pimpl_; }

	// On UNIX systems 'file_handle' and 'mapping_handle' are the same. On Windows,
	// however, a mapped region of a file gets its own handle, which is returned by
	// 'mapping_handle'.
	handle_type file_handle() const noexcept { return pimpl_->file_handle(); }
	handle_type mapping_handle() const noexcept { return pimpl_->mapping_handle(); }

	// Returns whether a valid memory mapping has been created.
	bool is_open() const noexcept { return pimpl_ && pimpl_->is_open(); }

	// Returns true if no mapping was established, that is, conceptually the
	// same as though the length that was mapped was 0. This function is
	// provided so that this class has Container semantics.
	bool empty() const noexcept { return !pimpl_ || pimpl_->empty(); }

	//
	// 'size' and 'length' both return the logical length, i.e. the number of bytes
	// user requested to be mapped, while 'mapped_length' returns the actual number of
	// bytes that were mapped which is a multiple of the underlying operating system's
	// page allocation granularity.
	//

	size_type size() const noexcept { return pimpl_ ? pimpl_->length() : 0; }
	size_type length() const noexcept { return pimpl_ ? pimpl_->length() : 0; }
	size_type mapped_length() const noexcept
	{
		return pimpl_ ? pimpl_->mapped_length() : 0;
	}

	// Returns the offset, relative to the file's start, at which the mapping was requested to be created.
	size_type offset() const noexcept { return pimpl_ ? pimpl_->offset() : 0; }

	// Returns a pointer to the first requested byte, or 'nullptr' if no memory mapping exists.
	template<access_mode A = AccessMode, typename = typename std::enable_if<A == access_mode::write>::type>
	pointer data() noexcept { return pimpl_->data(); }
	const_pointer data() const noexcept { return pimpl_ ? pimpl_->data() : nullptr; }

	//
	// Returns an iterator to the first requested byte, if a valid memory mapping
	// exists, otherwise this function call is undefined behaviour.
	//

	iterator begin() noexcept { return pimpl_->begin(); }
	const_iterator begin() const noexcept { return pimpl_->begin(); }
	const_iterator cbegin() const noexcept { return pimpl_->cbegin(); }

	// Returns an iterator one past the last requested byte, if a valid memory mapping
	// exists, otherwise this function call is undefined behaviour.
	template<access_mode A = AccessMode, typename = typename std::enable_if<A == access_mode::write>::type>
	iterator end() noexcept { return pimpl_->end(); }
	const_iterator end() const noexcept { return pimpl_->end(); }
	const_iterator cend() const noexcept { return pimpl_->cend(); }

	// Returns a reverse iterator to the last memory mapped byte, if a valid
	// memory mapping exists, otherwise this function call is undefined
	// behaviour.
	template<access_mode A = AccessMode, typename = typename std::enable_if<A == access_mode::write>::type>
	reverse_iterator rbegin() noexcept { return pimpl_->rbegin(); }
	const_reverse_iterator rbegin() const noexcept { return pimpl_->rbegin(); }
	const_reverse_iterator crbegin() const noexcept { return pimpl_->crbegin(); }

	// Returns a reverse iterator past the first mapped byte, if a valid memory
	// mapping exists, otherwise this function call is undefined behaviour.
	template<access_mode A = AccessMode, typename = typename std::enable_if<A == access_mode::write>::type>
	reverse_iterator rend() noexcept { return pimpl_->rend(); }
	const_reverse_iterator rend() const noexcept { return pimpl_->rend(); }
	const_reverse_iterator crend() const noexcept { return pimpl_->crend(); }

	// Returns a reference to the 'i'th byte from the first requested byte (as returned
	// by 'data'). If this is invoked when no valid memory mapping has been created
	// prior to this call, undefined behaviour ensues.
	reference operator[](const size_type i) noexcept { return (*pimpl_)[i]; }
	const_reference operator[](const size_type i) const noexcept { return (*pimpl_)[i]; }

	// Establishes a memory mapping with AccessMode. If the mapping is unsuccesful, the
	// reason is reported via 'error' and the object remains in a state as if this
	// function hadn't been called.
	//
	// 'path', which must be a path to an existing file, is used to retrieve a file
	// handle (which is closed when the object destructs or 'unmap' is called), which is
	// then used to memory map the requested region. Upon failure, 'error' is set to
	// indicate the reason and the object remains in an unmapped state.
	//
	// 'offset' is the number of bytes, relative to the start of the file, where the
	// mapping should begin. When specifying it, there is no need to worry about
	// providing a value that is aligned with the operating system's page allocation
	// granularity. This is adjusted by the implementation such that the first requested
	// byte (as returned by 'data' or 'begin'), so long as 'offset' is valid, will be at
	// 'offset' from the start of the file.
	//
	// 'length' is the number of bytes to map. It may be 'map_entire_file', in which
	// case a mapping of the entire file is created.
	template<typename String>
	void map(const String& path, const size_type offset, const size_type length, std::error_code& error)
	{
		map_impl(path, offset, length, error);
	}

	// Establishes a memory mapping with AccessMode. If the mapping is unsuccesful, the
	// reason is reported via 'error' and the object remains in a state as if this
	// function hadn't been called.
	//
	// 'handle', which must be a valid file handle, which is used to memory map the
	// requested region. Upon failure, 'error' is set to indicate the reason and the
	// object remains in an unmapped state.
	//
	// 'offset' is the number of bytes, relative to the start of the file, where the
	// mapping should begin. When specifying it, there is no need to worry about
	// providing a value that is aligned with the operating system's page allocation
	// granularity. This is adjusted by the implementation such that the first requested
	// byte (as returned by 'data' or 'begin'), so long as 'offset' is valid, will be at
	// 'offset' from the start of the file.
	//
	// 'length' is the number of bytes to map. It may be 'map_entire_file', in which
	// case a mapping of the entire file is created.
	void map(const handle_type handle, const size_type offset, const size_type length, std::error_code& error)
	{
		map_impl(handle, offset, length, error);
	}

	// If a valid memory mapping has been created prior to this call, this call
	// instructs the kernel to unmap the memory region and disassociate this object
	// from the file.
	//
	// The file handle associated with the file that is mapped is only closed if the
	// mapping was created using a file path. If, on the other hand, an existing
	// file handle was used to create the mapping, the file handle is not closed.
	void unmap() { if (pimpl_) pimpl_->unmap(); }

	void swap(BasicSharedMMap& other) { pimpl_.swap(other.pimpl_); }

	// Flushes the memory mapped page to disk. Errors are reported via 'error'.
	template< access_mode A = AccessMode, typename = typename std::enable_if<A == access_mode::write>::type>
	void sync(std::error_code& error) { if (pimpl_) pimpl_->sync(error); }

	//
	// All operators compare the underlying 'BasicMMap''s addresses.
	//

	friend bool operator==(const BasicSharedMMap& a, const BasicSharedMMap& b)
	{
		return a.pimpl_ == b.pimpl_;
	}

	friend bool operator!=(const BasicSharedMMap& a, const BasicSharedMMap& b)
	{
		return !(a == b);
	}

	friend bool operator<(const BasicSharedMMap& a, const BasicSharedMMap& b)
	{
		return a.pimpl_ < b.pimpl_;
	}

	friend bool operator<=(const BasicSharedMMap& a, const BasicSharedMMap& b)
	{
		return a.pimpl_ <= b.pimpl_;
	}

	friend bool operator>(const BasicSharedMMap& a, const BasicSharedMMap& b)
	{
		return a.pimpl_ > b.pimpl_;
	}

	friend bool operator>=(const BasicSharedMMap& a, const BasicSharedMMap& b)
	{
		return a.pimpl_ >= b.pimpl_;
	}

private:
	template<typename MappingToken>
	void map_impl(const MappingToken& token, const size_type offset, const size_type length, std::error_code& error)
	{
		if (!pimpl_) {
			mmap_type mmap = make_mmap<mmap_type>(token, offset, length, error);
			if (error) { return; }
			pimpl_ = std::make_shared<mmap_type>(std::move(mmap));
		} 
		else {
			pimpl_->map(token, offset, length, AccessMode, error);
		}
	}
};

// This is the basis for all read-only mmap objects and should be preferred over
// directly using BasicSharedMMap.
template<typename ByteType>
using basic_shared_mmap_source = BasicSharedMMap<access_mode::read, ByteType>;

// This is the basis for all read-write mmap objects and should be preferred over
// directly using BasicSharedMMap.
template<typename ByteType>
using basic_shared_mmap_sink = BasicSharedMMap<access_mode::write, ByteType>;

// These aliases cover the most common use cases, both representing a raw byte stream
// (either with a char or an unsigned char/uint8_t).
using shared_mmap_source = basic_shared_mmap_source<char>;
using shared_ummap_source = basic_shared_mmap_source<unsigned char>;

using shared_mmap_sink = basic_shared_mmap_sink<char>;
using shared_ummap_sink = basic_shared_mmap_sink<unsigned char>;

} // namespace Cry
} // namespace MemoryMap
