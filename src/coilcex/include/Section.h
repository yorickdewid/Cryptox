// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "OSAdapter.h"
#include "Executable.h"

#include <string>
#include <vector>
#include <bitset>

#define ILLEGAL_OFFSET -1

#define STORAGE_OPTION_ENCRYPTION 0
#define STORAGE_OPTION_COMPRESSION 1

namespace CryExe
{

using ByteArray = std::vector<std::uint8_t>;

class COILCEXAPI Section
{
	friend class Executable;
	friend struct Meta;

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
	std::bitset<2> storageOptionFlags;

public:
	using value_type = ByteArray::value_type;
	using size_type = ByteArray::size_type;
	using const_iterator = ByteArray::const_iterator;

private:
	struct COILCEXAPI DataPosition
	{
		DataPosition() = default;
		DataPosition(OSFilePosition offset, size_t size)
			: internalImageDataOffset{ offset }
			, internalImageDataSize{ size }
		{
		}

		OSFilePosition internalImageDataOffset = ILLEGAL_OFFSET;
		size_t internalImageDataSize = 0;
	} m_dataPosition;

protected:
	enum class DataSwapDirection { DATA_SWAP_IN, DATA_SWAP_OUT };
	virtual void DataSwap(DataSwapDirection) {}

public:
	Section(SectionType _type)
		: type{ _type }
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
	inline ByteArray Data() { return data; }

	// Section storage options
	inline void SetStorageOptionEncryption() { storageOptionFlags.set(STORAGE_OPTION_ENCRYPTION); }
	inline void SetStorageOptionCompression() { storageOptionFlags.set(STORAGE_OPTION_COMPRESSION); }

	//TODO
	// Internal offsets required to retrieve data from image
	inline OSFilePosition InternalDataOffset() const { return m_dataPosition.internalImageDataOffset; }
	inline size_t InternalDataSize() const { return m_dataPosition.internalImageDataSize; }

	// Iterator actions
	inline const_iterator begin() const { return data.begin(); }
	inline const_iterator end() const { return data.end(); }
	inline const_iterator cbegin() const { return data.begin(); }
	inline const_iterator cend() const { return data.end(); }

	// Capacity operations
	virtual inline void Clear() { data.clear(); }
	virtual inline bool Empty() const { return data.empty(); }
	virtual inline ByteArray::size_type Size() const { return data.size(); }

	// Append operators
	void operator<<(ByteArray::value_type value) { data.push_back(value); }
	void operator<<(const std::string& str) { data.insert(this->cend(), str.begin(), str.end()); }
	void operator+=(const std::string& str) { data.insert(this->cend(), str.begin(), str.end()); }
};

} // namespace CryExe
