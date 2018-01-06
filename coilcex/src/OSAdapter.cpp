// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "OSAdapter.h"

#include <cassert>
#include <functional>

void CryExe::OSAdapter::Open(const std::string& file, const char mode[])
{
	if (m_fpImage) { return; }

#ifdef _WIN32
	errno_t ret = fopen_s(&m_fpImage, file.c_str(), mode);
	assert(ret == 0);
#else
	m_fpImage = std::fopen(file.c_str(), mode);
#endif

	assert(m_fpImage);
}

void CryExe::OSAdapter::Close()
{
	if (!m_fpImage) { return; }

	int ret = std::fclose(m_fpImage);
	assert(ret == 0);
	m_fpImage = nullptr;
}

void CryExe::OSAdapter::Flush()
{
	if (!m_fpImage) { return; }

	int ret = std::fflush(m_fpImage);
	assert(ret == 0);
}

void CryExe::OSAdapter::Rewind()
{
	std::rewind(m_fpImage);
	m_fpOffset = 0;
}

void CryExe::OSAdapter::Forward()
{
	if (!m_fpImage) { return; }

	int ret = std::fseek(m_fpImage, 0, SEEK_END);
	assert(ret == 0);
	ret = std::fgetpos(m_fpImage, &m_fpOffset);
	assert(ret == 0);
	assert(m_fpOffset > 0);
}

void CryExe::OSAdapter::Position(std::fpos_t pos)
{
	if (!m_fpImage) { return; }
	
	if (pos == 0) {
		this->Rewind();
	}
	else {
		int ret = std::fsetpos(m_fpImage, &pos);
		assert(ret == 0);
	}
}

void CryExe::OSAdapter::ReadRaw(void *buffer, size_t size, size_t count)
{
	if (!m_fpImage) { return; }

	size_t sz = std::fread(buffer, size, count, m_fpImage);
	assert(sz == count);
}

void CryExe::OSAdapter::WriteRaw(const void *buffer, size_t size, size_t count)
{
	if (!m_fpImage) { return; }

	size_t sz = std::fwrite(buffer, size, count, m_fpImage);
	assert(sz == count);
	m_fpOffset += (size * count);
}

CryExe::OSAdapter::~OSAdapter()
{
	this->Close();
}
