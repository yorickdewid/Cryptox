// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/Program/Program.h>

#include <iostream>

#ifdef CRY_DEBUG
# define CRY_DEBUG_TRACE CRY_DEBUG_TRACE_ALL || 1
#endif

namespace CryCC
{
namespace Program
{

Program::Program(AST::AST&& tree)
	: m_ast{ new AST::AST{ std::move(tree) } }
{
}

Program::Program(Program&& other, AST::ASTNodeType&& ast)
	: m_ast{ new AST::AST{ std::move(ast) } }
	, m_treeCondition{ other.m_treeCondition }
	, m_lastStage{ other.m_lastStage }
	, m_locked{ other.m_locked }
{
}

#ifdef CRY_DEBUG
void Program::PrintSymbols()
{
	for (const auto& node : m_symbols) {
		std::cout << "Symbol: " << node.first << std::endl;
	}
}
#endif // CRY_DEBUG

bool Program::MatchSymbol(const std::string& symbol)
{
	return m_symbols.find(symbol) != m_symbols.cend();
}

Program::ResultSection& Program::GetResultSection(ResultSection::Tag tag)
{
	// If set is empty, insert element and return.
	if (m_resultSet.empty()) {
		auto it = m_resultSet.emplace(m_resultSet.cend(), tag);
		return (*it);
	}

	// Find resulting section based on tag.
	auto IsRulsetPresent = [=](const auto& tag) -> std::pair<bool, std::vector<ResultSection>::iterator>
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
		auto rs = IsRulsetPresent(ResultSection::AIIPX);
		if (rs.first) { return (*rs.second); }
		break;
	}
	case ResultSection::CASM: {
		auto rs = IsRulsetPresent(ResultSection::CASM);
		if (rs.first) { return (*rs.second); }
		break;
	}
	}

	// Create new instance of the tag.
	auto it = m_resultSet.emplace(m_resultSet.cend(), tag);
	return (*it);
}

} // namespace Program
} // namespace CryCC
