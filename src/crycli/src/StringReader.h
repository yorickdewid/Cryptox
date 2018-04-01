// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Reader.h"

#define SOURCE_NAME "__MEMORY__"

class VirtualSourceUnit : public SourceUnit
{
public:
	VirtualSourceUnit(const std::string& name, const std::string& content)
		: SourceUnit{ name }
		, m_content{ content }
	{
	}

	VirtualSourceUnit(const std::string& name, std::string&& content)
		: SourceUnit{ name }
		, m_content{ std::move(content) }
	{
	}

	// All local members can be moved implicitly
	VirtualSourceUnit(const VirtualSourceUnit&) = default;
	VirtualSourceUnit(VirtualSourceUnit&&) = default;

	// There is nothing to close, but prevent base to close file descriptor
	virtual inline void Close() override
	{
	}

	// Return the size of the memory allocated code stub
	virtual inline size_t Size() const override
	{
		return m_content.size();
	}

	virtual const std::string Read(size_t size) override
	{
		auto part = m_content.substr(offset, size);
		offset += part.size();

		return part;
	}

protected:
	std::string m_content;

private:
	size_t offset = 0;
};

struct StringReader : public FileReader
{
	StringReader(const std::string& content)
	{
		AppendFileToList(VirtualSourceUnit(SOURCE_NAME, content));
	}

	StringReader(std::string&& content)
	{
		AppendFileToList(VirtualSourceUnit(SOURCE_NAME, std::move(content)));
	}
};