// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Program.h"

#include <iostream>

#ifdef CRY_DEBUG
# define CRY_DEBUG_TRACE CRY_DEBUG_TRACE_ALL || 1
#endif

using namespace CoilCl;

template<int Increment = 1>
void PhaseInc(Program::ConditionTracker::ProgramPhase& phase)
{
	phase = static_cast<Program::ConditionTracker::ProgramPhase>(static_cast<int>(phase) + Increment);
}

// Advance or jump the program phase.
void Program::ConditionTracker::Advance(ProgramPhase jump)
{
	if (m_treeCondition == Program::ConditionTracker::ProgramPhase::_MAX) {
		return;
	}

	if (jump == _INVAL) {
		PhaseInc(m_treeCondition);
	}
	else if (jump > m_treeCondition) {
		m_treeCondition = jump;
	}

#ifdef CRY_DEBUG_TRACE
	std::cout << "phase " << m_treeCondition << " done" << std::endl;
#endif
}

void Program::PrintSymbols()
{
	for (const auto& node : m_symbols) {
		std::cout << "Symbol: " << node.first << std::endl;
	}
}

bool Program::MatchSymbol(const std::string& symbol)
{
	return m_symbols.find(symbol) != m_symbols.cend();
}

Program::ResultSection& Program::GetResultSection(ResultSection::Tag tag)
{
	// If set is empty, insert element and return
	if (m_resultSet.empty()) {
		auto it = m_resultSet.emplace(m_resultSet.cend(), tag);
		return (*it);
	}

	// Find resulting section based on tag
	auto isRulsetPresent = [=](const auto& tag) -> std::pair<bool, std::vector<ResultSection>::iterator>
	{
		auto it = std::find_if(m_resultSet.begin(), m_resultSet.end(), [&tag](const ResultSection& res)
		{
			return res.m_tag == tag;
		});

		return { it != m_resultSet.end(), it };
	};

	// Some tags allow for only one section. If it is already filled, return
	// the section so that additional data can be appended to the section. The
	// tags listed below only allow for single content.
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

	// Create new instance of the tag
	auto it = m_resultSet.emplace(m_resultSet.cend(), tag);
	return (*it);
}
