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
	ByteArray data;

public:
	using value_type = ByteArray::value_type;
	using size_type = ByteArray::size_type;
	using iterator = ByteArray::iterator;
	using const_iterator = ByteArray::const_iterator;

public:
	void Emplace(const std::vector<std::uint8_t>& bstream)
	{
		data = bstream;
	}

	void Emplace(std::vector<std::uint8_t>&& bstream)
	{
		data = std::move(bstream);
	}

	// Add one element to the buffer
	void PusbBack(ByteArray::value_type& value) { data.push_back(value); }
	void PusbBack(ByteArray::value_type&& value) { data.push_back(std::move(value)); }

	// Convert string to byte array
	inline void Emplace(const std::string& sstream)
	{
		std::vector<std::uint8_t> vec{ sstream.begin(), sstream.end() };
		this->Emplace(std::move(vec));
	}

	// Return data object
	inline ByteArray Data() const { return data; }
	inline ByteArray::size_type Size() const { return data.size(); }

	// Iterator types
	inline iterator begin() { return data.begin(); }
	inline iterator end() { return data.end(); }
	inline const_iterator begin() const { return data.begin(); }
	inline const_iterator end() const { return data.end(); }
	inline const_iterator cbegin() const { return data.begin(); }
	inline const_iterator cend() const { return data.end(); }

	// 
	void operator<<(ByteArray& bstream)
	{
		data.insert(this->cend(), bstream.begin(), bstream.end());
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
