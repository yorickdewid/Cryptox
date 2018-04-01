// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Reader.h"
#include "SourceUnit.h"

#include <boost/filesystem.hpp>

#include <stack>

class FileReader : public Reader
{
public:
	FileReader() = default;

	FileReader(const std::string& filename)
	{
		AppendFileToList(filename);
	}

	// Implement interface reader
	virtual std::string FetchNextChunk(size_t sizeHint)
	{
		auto content = m_unitList.top()->Read(sizeHint);
		if (content.empty()) {
			m_unitList.pop();
		}

		return content;
	}

	// Implement interface meta info request
	virtual std::string FetchMetaInfo()
	{
		return m_unitList.top()->Name();
	}

	// Implement interface, switch to source
	virtual void SwitchSource(const std::string& source)
	{
		AppendFileToList(source);
	}

protected:
	void AppendFileToList(const std::string& filename)
	{
		//XXX: Switch to C++17
		if (!boost::filesystem::exists(filename)) {
			throw std::system_error{ std::make_error_code(std::errc::no_such_file_or_directory) };
		}

		m_unitList.push(std::make_unique<SourceUnit>(SourceUnit{ filename }));
	}

	// Append source unit to unit stack
	void AppendFileToList(SourceUnit&& unit)
	{
		m_unitList.push(std::make_unique<SourceUnit>(std::move(unit)));
	}

private:
	std::stack<std::unique_ptr<SourceUnit>> m_unitList;
};

