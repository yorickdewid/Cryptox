// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <functional>

class SectionTable
{
	struct MetaSection;
	using SectionCallback = std::function<void(MetaSection&)>;

private:
	const CryExe::Executable& m_exec;

private:
	struct MetaSection
	{
		std::string name;
		std::string type;
		long long offset;
		size_t size;
		char flags[16];
	};
	
	std::string SectionRow(MetaSection, int);

	void ForEach(SectionCallback func);

	bool HasSections() const;

private:
	static std::string TableLegend();

public:
	SectionTable(const CryExe::Executable&);

	static void ParseTable(const CryExe::Executable& exec);
};
