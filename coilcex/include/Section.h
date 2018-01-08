// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Executable.h"

#include <string>
#include <vector>

#define ILLEGAL_OFFSET -1

namespace CryExe
{

using ByteArray = std::vector<std::uint8_t>;

class COILCEXAPI Section
{
public:
	enum class COILCEXAPI SectionType
	{
		NATIVE,
		RESOURCE,
		DATA,
		DEBUG,
		SOURCE,
		NOTE,
	};

private:
	ByteArray data;
	SectionType type;

public:
	using value_type = ByteArray::value_type;
	using size_type = ByteArray::size_type;
	using const_iterator = ByteArray::const_iterator;

private:
	struct COILCEXAPI DataPosition
	{
		DataPosition() = default;
		DataPosition(std::fpos_t offset, size_t size)
			: internalImageDataOffset{ offset }
			, internalImageDataSize{ size }
		{
		}

		std::fpos_t internalImageDataOffset = ILLEGAL_OFFSET;
		size_t internalImageDataSize = 0;
	} m_dataPosition;

public:
	Section(SectionType _type)
		: type{ _type }
	{
	}

	Section(SectionType _type, std::fpos_t offset, size_t size)
		: m_dataPosition{ offset, size }
		, type{ _type }
	{
	}

	// Insert data into section
	void Emplace(const ByteArray& bstream) { data = bstream; }
	void Emplace(ByteArray&& bstream) { data = std::move(bstream); }

	// Convert string to byte array
	inline void Emplace(const std::string& sstream)
	{
		ByteArray vec{ sstream.begin(), sstream.end() };
		this->Emplace(std::move(vec));
	}

	// Add one element to the buffer
	void PusbBack(ByteArray::value_type& value) { data.push_back(value); }
	void PusbBack(ByteArray::value_type&& value) { data.push_back(std::move(value)); }

	// Return data object
	inline SectionType Type() const { return type; }
	inline const ByteArray& Data() const { return data; }

	// Internal offsets required to retrieve data from image
	inline std::fpos_t InternalDataOffset() const { return m_dataPosition.internalImageDataOffset; }
	inline size_t InternalDataSize() const { return m_dataPosition.internalImageDataSize; }

	// Iterator types
	inline const_iterator begin() const { return data.begin(); }
	inline const_iterator end() const { return data.end(); }
	inline const_iterator cbegin() const { return data.begin(); }
	inline const_iterator cend() const { return data.end(); }

	// Capacity operations
	inline void Clear() { data.clear(); }
	inline bool Empty() const { return data.empty(); }
	inline ByteArray::size_type Size() const { return data.size(); }

	// 
	void operator<<(ByteArray::value_type value)
	{
		data.push_back(value);
	}

	//
	void operator<<(const std::string& str)
	{
		data.insert(this->cend(), str.begin(), str.end());
	}

	//
	void operator+=(const std::string& str)
	{
		data.insert(this->cend(), str.begin(), str.end());
	}
};

} // namespace CryExe
