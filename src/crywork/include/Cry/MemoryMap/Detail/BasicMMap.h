// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/Cry.h>

#include <iterator>
#include <string>
#include <system_error>

namespace Cry
{
namespace MemoryMap
{
namespace Detail
{

enum { MAP_ENTIRE_FILE };

#ifdef CRY_WINDOWS
using file_handle_type = HANDLE;
#else
using file_handle_type = int;
#endif

template<typename ByteType>
struct BasicMMap
{
	using value_type = ByteType;
	using size_type = int64_t;
	using reference = value_type & ;
	using const_reference = const value_type&;
	using pointer = value_type * ;
	using const_pointer = const value_type*;
	using difference_type = std::ptrdiff_t;
	using iterator = pointer;
	using const_iterator = const_pointer;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using iterator_category = std::random_access_iterator_tag;
	using handle_type = file_handle_type;

	static_assert(sizeof(ByteType) == sizeof(char), "ByteType must be the same size as char.");

private:
	// Points to the first requested byte, and not to the actual start of the mapping.
	pointer m_data{ nullptr };

	// Length, in bytes, requested by user, which may not be the length of the full mapping.
	size_type m_length{ 0 };
	// Lengthof the full mapping.
	size_type m_mappedLength{ 0 };

	// Letting user map a file using both an existing file handle and a path introcudes
	// On POSIX, we only need a file handle to create a mapping, while on Windows
	// systems the file handle is necessary to retrieve a file mapping handle, but any
	// subsequent operations on the mapped region must be done through the latter.
	handle_type file_handle_{ INVALID_HANDLE_VALUE };
#ifdef CRY_WINDOWS
	handle_type file_mapping_handle_{ INVALID_HANDLE_VALUE };
#endif

	// some complexity in that we must not close the file handle if user provided it,
	// but we must close it if we obtained it using the provided path. For this reason,
	// this flag is used to determine when to close file_handle_.
	bool m_is_handle_internal;

public:
	BasicMMap() = default;
	BasicMMap(const BasicMMap&) = delete;
	BasicMMap& operator=(const BasicMMap&) = delete;

	BasicMMap(BasicMMap&& other)
		: m_data{ std::move(other.m_data) }
		, m_length{ std::move(other.m_length) }
		, m_mappedLength{ std::move(other.m_mappedLength) }
		, file_handle_{ std::move(other.file_handle_) }
#ifdef CRY_WINDOWS
		, file_mapping_handle_{ std::move(other.file_mapping_handle_) }
#endif
		, m_is_handle_internal{ std::move(other.m_is_handle_internal) }
	{
		other.m_data = nullptr;
		other.m_length = other.m_mappedLength = 0;
		other.file_handle_ = INVALID_HANDLE_VALUE;
#ifdef CRY_WINDOWS
		other.file_mapping_handle_ = INVALID_HANDLE_VALUE;
#endif
	}

	BasicMMap& operator=(BasicMMap&& other)
	{
		if (this == &other) { return (*this); }

		// First the existing mapping needs to be removed.
		unmap();

		m_data = std::move(other.m_data);
		m_length = std::move(other.m_length);
		m_mappedLength = std::move(other.m_mappedLength);
		file_handle_ = std::move(other.file_handle_);
#ifdef CRY_WINDOWS
		file_mapping_handle_ = std::move(other.file_mapping_handle_);
#endif
		m_is_handle_internal = std::move(other.m_is_handle_internal);

		// The moved from basic_mmap's fields need to be reset, because otherwise other's
		// destructor will unmap the same mapping that was just moved into this.
		other.m_data = nullptr;
		other.m_length = other.m_mappedLength = 0;
		other.file_handle_ = INVALID_HANDLE_VALUE;
#ifdef CRY_WINDOWS
		other.file_mapping_handle_ = INVALID_HANDLE_VALUE;
#endif
		other.m_is_handle_internal = false;
	}

	~BasicMMap()
	{
		unmap();
	}

	handle_type file_handle() const noexcept { return file_handle_; }
	handle_type mapping_handle() const noexcept
	{
#ifdef CRY_WINDOWS
		return file_mapping_handle_;
#else
		return file_handle_;
#endif
	}

	bool is_open() const noexcept { return file_handle_ != INVALID_HANDLE_VALUE; }
	bool empty() const noexcept { return length() == 0; }
	bool is_mapped() const noexcept
	{
#ifdef CRY_WINDOWS
		return file_mapping_handle_ != INVALID_HANDLE_VALUE;
#else
		return is_open();
#endif
	}

	size_type offset() const noexcept { return m_mappedLength - m_length; }
	size_type length() const noexcept { return m_length; }
	size_type mapped_length() const noexcept { return m_mappedLength; }

	pointer data() noexcept { return m_data; }
	const_pointer data() const noexcept { return m_data; }

	iterator begin() noexcept { return data(); }
	const_iterator begin() const noexcept { return data(); }
	const_iterator cbegin() const noexcept { return data(); }

	iterator end() noexcept { return begin() + length(); }
	const_iterator end() const noexcept { return begin() + length(); }
	const_iterator cend() const noexcept { return begin() + length(); }

	reverse_iterator rbegin() { return reverse_iterator(end()); }
	const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
	const_reverse_iterator crbegin() const { return const_reverse_iterator(end()); }

	reverse_iterator rend() { return reverse_iterator(begin()); }
	const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
	const_reverse_iterator crend() const { return const_reverse_iterator(begin()); }

	reference operator[](const size_type i) noexcept { return m_data[i]; }
	const_reference operator[](const size_type i) const noexcept { return m_data[i]; }

	template<typename String>
	void map(String& path, size_type offset, size_type length, access_mode mode, std::error_code& error)
	{
		error.clear();
		if (detail::empty(path)) {
			error = std::make_error_code(std::errc::invalid_argument);
			return;
		}
		const auto handle = open_file(path, mode, error);
		if (error) { return; }
		map(handle, offset, length, mode, error);
		// This MUST be after the call to map, as that sets this to true.
		if (!error) {
			m_is_handle_internal = true;
		}
	}

	void map(handle_type handle, size_type offset, size_type length, access_mode mode, std::error_code& error)
	{
		error.clear();
		if (handle == INVALID_HANDLE_VALUE) {
			error = std::make_error_code(std::errc::bad_file_descriptor);
			return;
		}

		const auto file_size = query_file_size(handle, error);
		if (error) { return; }

		if (length <= map_entire_file) {
			length = file_size;
		}
		else if (offset + length > file_size) {
			error = std::make_error_code(std::errc::invalid_argument);
			return;
		}

		const mmap_context ctx = memory_map(handle, offset, length, mode, error);
		if (!error) {
			// We must unmap the previous mapping that may have existed prior to this call.
			// Note that this must only be invoked after a new mapping has been created in
			// order to provide the strong guarantee that, should the new mapping fail, the
			// `map` function leaves this instance in a state as though the function had
			// never been invoked.
			unmap();

			file_handle_ = handle;
			m_is_handle_internal = false;
			m_data = reinterpret_cast<pointer>(ctx.data);
			m_length = ctx.length;
			m_mappedLength = ctx.mapped_length;
#ifdef CRY_WINDOWS
			file_mapping_handle_ = ctx.file_mapping_handle;
#endif
		}
	}

	void unmap()
	{
		if (!is_open()) { return; }

		// TODO: do we care about errors here?
#ifdef CRY_WINDOWS
		if (is_mapped()) {
			::UnmapViewOfFile(MappingStartAt());
			::CloseHandle(file_mapping_handle_);
			file_mapping_handle_ = INVALID_HANDLE_VALUE;
		}
#else
		if (m_data) { ::munmap(const_cast<pointer>(MappingStartAt()), m_mappedLength); }
#endif

		// If file_handle_ was obtained by our opening it (when map is called with a path,
		// rather than an existing file handle), we need to close it, otherwise it must not
		// be closed as it may still be used outside this instance.
		if (m_is_handle_internal) {
#ifdef CRY_WINDOWS
			::CloseHandle(file_handle_);
#else
			::close(file_handle_);
#endif
		}

		// Reset fields to their default values.
		m_data = nullptr;
		m_length = m_mappedLength = 0;
		file_handle_ = INVALID_HANDLE_VALUE;
#ifdef CRY_WINDOWS
		file_mapping_handle_ = INVALID_HANDLE_VALUE;
#endif
	}

	void sync(std::error_code& error)
	{
		error.clear();
		if (!is_open()) {
			error = std::make_error_code(std::errc::bad_file_descriptor);
			return;
		}

		if (data()) {
#ifdef CRY_WINDOWS
			if (!::FlushViewOfFile(MappingStartAt(), m_mappedLength) || !::FlushFileBuffers(file_handle_)) {
#else
			if (::msync(MappingStartAt(), m_mappedLength, MS_SYNC) != 0) {
#endif
				error = last_error();
				return;
			}
		}

#ifdef CRY_WINDOWS
		if (!::FlushFileBuffers(file_handle_)) {
			error = last_error();
		}
#endif
	}

	void swap(BasicMMap& other)
	{
		if (this == &other) { return; }

		std::swap(m_data, other.m_data);
		std::swap(file_handle_, other.file_handle_);
#ifdef CRY_WINDOWS
		std::swap(file_mapping_handle_, other.file_mapping_handle_);
#endif
		std::swap(m_length, other.m_length);
		std::swap(m_mappedLength, other.m_mappedLength);
		std::swap(m_is_handle_internal, other.m_is_handle_internal);
	}

private:
	pointer MappingStartAt() noexcept { return !data() ? nullptr : data() - offset(); }
};

template<typename ByteType>
bool operator==(const BasicMMap<ByteType>& lhs, const BasicMMap<ByteType>& rhs)
{
	return lhs.data() == rhs.data() && lhs.size() == rhs.size();
}
template<typename ByteType>
bool operator!=(const BasicMMap<ByteType>& lhs, const BasicMMap<ByteType>& rhs)
{
	return !(lhs == rhs);
}
template<typename ByteType>
bool operator<(const BasicMMap<ByteType>& lhs, const BasicMMap<ByteType>& rhs)
{
	if (lhs.data() == rhs.data()) {
		return lhs.size() < rhs.size();
	}
	return lhs.data() < rhs.data();
}
template<typename ByteType>
bool operator<=(const BasicMMap<ByteType>& lhs, const BasicMMap<ByteType>& rhs)
{
	return !(lhs > rhs);
}
template<typename ByteType>
bool operator>(const BasicMMap<ByteType>& lhs, const BasicMMap<ByteType>& rhs)
{
	if (lhs.data() == rhs.data()) {
		return lhs.size() > rhs.size();
	}
	return lhs.data() > rhs.data();
}
template<typename ByteType>
bool operator>=(const BasicMMap<ByteType>& lhs, const BasicMMap<ByteType>& rhs)
{
	return !(lhs < rhs);
}

} // namespace Detail
} // namespace MemoryMap
} // namespace Cry
