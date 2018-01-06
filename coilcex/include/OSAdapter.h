// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Exportable.h"

#include <string>

namespace CryExe
{

class COILCEXAPI OSAdapter
{
	std::FILE *m_fpImage = nullptr;
	std::fpos_t m_fpOffset = 0;

public:
	OSAdapter() = default;
	~OSAdapter();

	bool IsOpen() const { return m_fpImage; }
	void Open(const std::string& file, const char mode[]);
	void Close();
	void Flush();
	void Rewind();
	void Forward();
	void Position(std::fpos_t);

	inline std::fpos_t Offset() const { return m_fpOffset ; }

	template<typename _Ty>
	inline void Read(_Ty& buffer, size_t size = sizeof(_Ty), size_t count = 1)
	{
		ReadRaw(static_cast<void *>(std::addressof(buffer)), size, count);
	}

	template<typename _Ty>
	inline void Write(_Ty& buffer, size_t size = sizeof(_Ty), size_t count = 1)
	{
		WriteRaw(static_cast<const void *>(std::addressof(buffer)), size, count);
	}

private:
	void ReadRaw(void *buffer, size_t size, size_t count);
	void WriteRaw(const void *buffer, size_t size, size_t count);
};

} // namespace CryExecutable
