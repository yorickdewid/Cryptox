// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

//#include "Exportable.h"
#include "Executable.h"

#include <string>
#include <vector>

namespace CryExe
{

using ByteArray = std::vector<std::uint8_t>;

class COILCEXAPI Section
{
public:
	enum SectionType
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

public:
	Section(SectionType _type)
		: type{ _type }
	{
	}

	void Emplace(const ByteArray& bstream)
	{
		data = bstream;
	}

	void Emplace(ByteArray&& bstream)
	{
		data = std::move(bstream);
	}

	// Add one element to the buffer
	void PusbBack(ByteArray::value_type& value) { data.push_back(value); }
	void PusbBack(ByteArray::value_type&& value) { data.push_back(std::move(value)); }

	// Convert string to byte array
	inline void Emplace(const std::string& sstream)
	{
		ByteArray vec{ sstream.begin(), sstream.end() };
		this->Emplace(std::move(vec));
	}

	// Return data object
	inline SectionType Type() const { return type; }
	inline ByteArray Data() const { return data; }

	// Iterator types
	inline const_iterator begin() const { return data.begin(); }
	inline const_iterator end() const { return data.end(); }
	inline const_iterator cbegin() const { return data.begin(); }
	inline const_iterator cend() const { return data.end(); }

	// Capacity operations
	inline void Clear() { data.clear(); }
	inline void Empty() { data.empty(); }
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

	//TODO:
	inline bool IsAllowedOnce() const { return true; }
};

} // namespace CryExe
