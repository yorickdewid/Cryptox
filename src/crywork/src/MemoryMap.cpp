// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <Cry/Cry.h>
#include <Cry/MemoryMap/Page.h>
#include <Cry/MemoryMap/BasicMMap.h>

#ifdef CRY_LINUX
# include <sys/mman.h>
# include <sys/stat.h>
#endif

namespace Cry
{
namespace MemoryMap
{

// Determines the operating system's page allocation granularity.
//
// On the first call to this function, it invokes the operating system specific syscall
// to determine the page size, caches the value, and returns it. Any subsequent call to
// this function serves the cached value, so no further syscalls are made.
size_t PageSize()
{
#ifdef CRY_WINDOWS
	SYSTEM_INFO SystemInfo;
	::GetSystemInfo(&SystemInfo);
	return SystemInfo.dwAllocationGranularity;
#else
	return ::sysconf(_SC_PAGE_SIZE);
#endif
}

// Alligns 'offset' to the operating's system page size such that it subtracts the
// difference until the nearest page boundary before 'offset', or does nothing if
// 'offset' is already page aligned.
size_t MakeOffsetPageAligned(size_t offset) noexcept
{
	// Use integer division to round down to the nearest page alignment.
	const size_t page_size = PageSize();
	return offset / page_size * page_size;
}

std::error_code LastNativeError() noexcept
{
	std::error_code error;
#ifdef CRY_WINDOWS
	error.assign(GetLastError(), std::system_category());
#else
	error.assign(errno, std::system_category());
#endif
	return error;
}

namespace Detail
{
	
size_t FileSizeHandle(file_handle_type handle, std::error_code& error)
{
	error.clear();

#ifdef CRY_WINDOWS
	LARGE_INTEGER file_size;
	if (!::GetFileSizeEx(handle, &file_size)) {
		error = LastNativeError();
		return 0;
	}

	return static_cast<size_t>(file_size.QuadPart);
#else
	struct stat sbuf;
	if (::fstat(handle, &sbuf) < 0) {
		error = LastNativeError();
		return 0;
	}

	return sbuf.st_size;
#endif
}

struct MMapRawResult
{
	char *data;
	int64_t length;
	int64_t mapped_length;
#ifdef CRY_WINDOWS
	file_handle_type file_mapping_handle;
#endif
};

MMapRawResult MMapRaw(const file_handle_type file_handle, const int64_t offset, const int64_t length, const AccessModeType mode, std::error_code& error)
{
	const int64_t aligned_offset = MakeOffsetPageAligned(offset);
	const int64_t length_to_map = offset - aligned_offset + length;
#ifdef CRY_WINDOWS
	const int64_t max_file_size = offset + length;

	const auto file_mapping_handle = ::CreateFileMapping(file_handle
		, 0
		, mode == AccessModeType::READ ? PAGE_READONLY : PAGE_READWRITE
		, INT64_HIGH(max_file_size)
		, INT64_LOW(max_file_size)
		, 0);
	if (file_mapping_handle == INVALID_HANDLE_VALUE) {
		error = LastNativeError();
		return {};
	}

	char *mapping_start = static_cast<char*>(::MapViewOfFile(file_mapping_handle
		, mode == AccessModeType::READ ? FILE_MAP_READ : FILE_MAP_WRITE
		, INT64_HIGH(aligned_offset)
		, INT64_LOW(aligned_offset)
		, length_to_map));
	if (!mapping_start) {
		error = LastNativeError();
		return {};
	}
#else
	char* mapping_start = static_cast<char*>(::mmap(
		0, // Don't give hint as to where to map.
		length_to_map,
		mode == AccessModeType::READ ? PROT_READ : PROT_WRITE,
		MAP_SHARED,
		file_handle,
		aligned_offset));
	if (mapping_start == MAP_FAILED) {
		error = LastNativeError();
		return {};
	}
#endif

	MMapRawResult ctx;
	ctx.data = mapping_start + offset - aligned_offset;
	ctx.length = length;
	ctx.mapped_length = length_to_map;
#ifdef CRY_WINDOWS
	ctx.file_mapping_handle = file_mapping_handle;
#endif
	return ctx;
}

BasicMMap::BasicMMap(BasicMMap&& other)
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

BasicMMap::~BasicMMap()
{
	unmap();
}

BasicMMap& BasicMMap::operator=(BasicMMap&& other)
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

	return (*this);
}

bool BasicMMap::is_mapped() const noexcept
{
#ifdef CRY_WINDOWS
	return file_mapping_handle_ != INVALID_HANDLE_VALUE;
#else
	return is_open();
#endif
}

void BasicMMap::map(handle_type handle, size_type offset, size_type length, AccessModeType mode, std::error_code& error)
{
	error.clear();
	if (handle == INVALID_HANDLE_VALUE) {
		error = std::make_error_code(std::errc::bad_file_descriptor);
		return;
	}

	const size_type fileSize = static_cast<size_type>(FileSizeHandle(handle, error));
	if (error) { return; }

	if (length <= MAP_ENTIRE_FILE) {
		length = fileSize;
	}
	else if (offset + length > fileSize) {
		error = std::make_error_code(std::errc::invalid_argument);
		return;
	}

	const MMapRawResult ctx = MMapRaw(handle, offset, length, mode, error);
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

void BasicMMap::unmap()
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

void BasicMMap::sync(std::error_code& error)
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
			error = LastNativeError();
			return;
		}
	}

#ifdef CRY_WINDOWS
	if (!::FlushFileBuffers(file_handle_)) {
		error = LastNativeError();
	}
#endif
}

void BasicMMap::swap(BasicMMap& other)
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

bool operator==(const BasicMMap& lhs, const BasicMMap& rhs)
{
	return lhs.data() == rhs.data() && lhs.mapped_length() == rhs.mapped_length();
}
bool operator!=(const BasicMMap& lhs, const BasicMMap& rhs)
{
	return !(lhs == rhs);
}
bool operator<(const BasicMMap& lhs, const BasicMMap& rhs)
{
	if (lhs.data() == rhs.data()) {
		return lhs.mapped_length() < rhs.mapped_length();
	}
	return lhs.data() < rhs.data();
}
//bool operator<=(const BasicMMap& lhs, const BasicMMap& rhs)
//{
//	return !(lhs > rhs);
//}
bool operator>(const BasicMMap& lhs, const BasicMMap& rhs)
{
	if (lhs.data() == rhs.data()) {
		return lhs.mapped_length() > rhs.mapped_length();
	}
	return lhs.data() > rhs.data();
}
//bool operator>=(const BasicMMap& lhs, const BasicMMap& rhs)
//{
//	return !(lhs < rhs);
//}

} // namespace Detail
} // namespace MemoryMap
} // namespace Cry
