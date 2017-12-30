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
	fopen_s(&m_fpImage, file.c_str(), mode);
#else
	m_fpImage = std::fopen(file.c_str(), mode);
#endif

	assert(m_fpImage);
}

void CryExe::OSAdapter::Close()
{
	if (!m_fpImage) { return; }

	assert(std::fclose(m_fpImage) == 0);
	m_fpImage = nullptr;
}

void CryExe::OSAdapter::Flush()
{
	if (!m_fpImage) { return; }

	assert(std::fflush(m_fpImage) == 0);
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
}

CryExe::OSAdapter::~OSAdapter()
{
	this->Close();
}
