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
#include <system_error>

void CryExe::OSAdapter::Open(const std::string& file, const char mode[])
{
	if (m_fpImage) { return; }

#ifdef _WIN32
	errno_t ret = fopen_s(&m_fpImage, file.c_str(), mode);
	if (ret) { throw std::system_error{ ret, std::system_category() }; }
#else
	m_fpImage = std::fopen(file.c_str(), mode);
	if (!m_fpImage) { throw std::system_error{ errno, std::system_category() }; }
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
	long int pos = std::ftell(m_fpImage);
	assert(pos >= 0);
	assert(m_fpOffset > OSFilePosition{ 0 });
	m_fpOffset = pos;

	//TODO:
	//ret = std::fgetpos(m_fpImage, &_pos);
}

void CryExe::OSAdapter::Position(const OSFilePosition& pos)
{
	if (!m_fpImage) { return; }

	if (pos == OSFilePosition{ 0 }) {
		this->Rewind();
	}
	else {
		int ret = std::fseek(m_fpImage, pos.Position<long int>(), SEEK_SET);
		assert(ret == 0);

		//TODO:
		//int ret = std::fsetpos(m_fpImage, &pos);
	}
}

void CryExe::OSAdapter::ReadRaw(void *buffer, size_t size, size_t count)
{
	if (!m_fpImage) { return; }

	size_t sz = std::fread(buffer, size, count, m_fpImage);
	assert(sz == count); //TODO: read again
}

void CryExe::OSAdapter::WriteRaw(const void *buffer, size_t size, size_t count)
{
	if (!m_fpImage) { return; }

	size_t sz = std::fwrite(buffer, size, count, m_fpImage);
	assert(sz == count); //TODO: write again
	m_fpOffset += static_cast<OSFilePosition>(size * count);
}

CryExe::OSAdapter::~OSAdapter()
{
	this->Close();
}
