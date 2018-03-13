// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Program.h"

#include <iostream>

using namespace CoilCl;

void Program::ConditionTracker::Advance()
{
	//TODO
}

void Program::PrintSymbols()
{
	for (const auto& node : m_symbols) {
		std::cout << "Symbol: " << node.first << std::endl;
	}
}

Program::ResultSection& Program::GetResultSection(ResultSection::Tag tag)
{
	if (m_resultSet.empty()) {
		auto it = m_resultSet.emplace(m_resultSet.cend(), tag);
		return (*it);
	}

	auto isRulsetPresent = [=](const auto& tag) -> std::pair<bool, std::vector<ResultSection>::iterator>
	{
		auto it = std::find_if(m_resultSet.begin(), m_resultSet.end(), [&tag](const ResultSection& res)
		{
			return res.m_tag == tag;
		});

		return { it != m_resultSet.end(), it };
	};

	// Some tags allow for only one section. If it is already filled, return
	// the section so that additional data can be appended to the section.
	switch (tag)
	{
	case ResultSection::AIIPX: {
		auto rs = isRulsetPresent(ResultSection::AIIPX);
		if (rs.first) { return (*rs.second); }
		break;
	}
	case ResultSection::CASM: {
		auto rs = isRulsetPresent(ResultSection::CASM);
		if (rs.first) { return (*rs.second); }
		break;
	}
	}

	auto it = m_resultSet.emplace(m_resultSet.cend(), tag);
	return (*it);
}
