// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <Cry/Cry.h>
#include <Cry/Types.h>

#include <iterator>
#include <string>
#include <system_error>

namespace Cry
{
namespace MemoryMap
{
namespace Detail
{

enum { MAP_ENTIRE_FILE = 0, };

#ifdef CRY_WINDOWS
using file_handle_type = HANDLE;
#else
using file_handle_type = int;
#endif

struct BasicMMap
{
	using value_type = Cry::Byte;
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

private:
	// Points to the first requested byte, and not to the actual start of the mapping.
	pointer m_data{ nullptr };

	// Length, in bytes, requested by user, which may not be the length of the full mapping.
	size_type m_length{ 0 };
	// Length, in bytes of the full mapping.
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

	BasicMMap(BasicMMap&&);

	~BasicMMap();

	BasicMMap& operator=(BasicMMap&&);

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
	bool is_mapped() const noexcept;

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
	void map(String& path, size_type offset, size_type length, AccessModeType mode, std::error_code& error)
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

	void map(handle_type handle, size_type offset, size_type length, AccessModeType mode, std::error_code& error);
	void unmap();

	void sync(std::error_code& );
	void swap(BasicMMap& );

private:
	pointer MappingStartAt() noexcept { return !data() ? nullptr : data() - offset(); }
};

bool operator==(const BasicMMap&, const BasicMMap&);
bool operator!=(const BasicMMap&, const BasicMMap&);
bool operator<(const BasicMMap&, const BasicMMap&);
//bool operator<=(const BasicMMap&, const BasicMMap&);
bool operator>(const BasicMMap&, const BasicMMap&);
//bool operator>=(const BasicMMap&, const BasicMMap&);

} // namespace Detail
} // namespace MemoryMap
} // namespace Cry
