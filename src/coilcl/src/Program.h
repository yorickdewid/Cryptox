// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "AST.h"
#include "Stage.h"

#include <map>
#include <vector>
#include <memory>

namespace CoilCl
{

// Program is the resulting structure for all compiler stages.
// Only stages are allowed to alter the program internals. The
// program is passed to a program runner in which case the program
// is run, or the program structure is dissected in an analyzer.
class Program final
{
public:
	class ConditionTracker
	{
		//TODO: Shiftable enum
		enum
		{
			_INVAL = 0,
			CANONICAL,
			FUNDAMENTAL,
			STATIC_RESOLVED,
			ASSERTION_PASSED,
			COMPLANT,
			OPTIMIZED,
			STRIPPED,
			_MAX = STRIPPED,
		} m_treeCondition = _INVAL;

	public:
		// Check program status
		inline auto IsRunnable() const { return m_treeCondition >= ASSERTION_PASSED; }
		inline auto IsLanguage() const { return m_treeCondition >= COMPLANT; }
		inline auto IsOptimized() const { return m_treeCondition >= OPTIMIZED; }

		void Advance();

		ConditionTracker& operator++()
		{
			Advance();
			return (*this);
		}
	};

	class ResultSection
	{
		std::vector<uint8_t> m_content;

	public:
		enum Tag
		{
			AIIPX,         // Resulting section for AIIPX content
			CASM,          // Resulting section for CASM content
			COMPLEMENTARY, // Resulting section for additional content
		} m_tag;

	public:
		ResultSection(Tag tag = Tag::COMPLEMENTARY)
			: m_tag{ tag }
		{
		}

		inline size_t Size() const noexcept
		{
			return m_content.size();
		}

		std::vector<uint8_t>& Data()
		{
			return m_content;
		}
	};

public:
	// Program constructors
	Program() = default;
	Program(const Program&) = delete;
	Program(Program&&) = default;
	Program(AST::AST&& tree)
		: m_ast{ new AST::AST{ std::move(tree) } }
	{
	}
	Program(Program&& other, std::shared_ptr<TranslationUnitDecl>&& ast)
		: m_ast{ new AST::AST{ std::move(ast) } }
		, m_treeCondition{ other.m_treeCondition }
		, m_lastStage{ other.m_lastStage }
	{
	}

	// Do not assign
	Program& operator=(const Program&) = delete;
	Program& operator=(Program&&) = delete;

	// AST operations
	inline auto Ast() { return m_ast->tree_ref(); }
	inline auto AstPassthrough() const { return m_ast->operator->(); }

	// Symbol operations
	void PrintSymbols();
	inline bool HasSymbols() const { return !m_symbols.empty(); }
	inline bool HasSymbol(const std::string& name) const { return m_symbols.find(name) != m_symbols.end(); }
	auto& FillSymbols() { return m_symbols; } //TODO: friend?

	// Get memory block
	ResultSection& GetResultSection(ResultSection::Tag tag = ResultSection::Tag::COMPLEMENTARY);

	// Retieve program condition
	inline const ConditionTracker& Condition() const { return m_treeCondition; }

	template<typename... _ArgsTy>
	static void Bind(std::unique_ptr<Program>&& program, _ArgsTy&&... args)
	{
		auto ptr = program.release();
		program = std::make_unique<Program>(std::move(*(ptr)), std::forward<_ArgsTy>(args)...);
		delete ptr;
	}

private:
	ConditionTracker m_treeCondition;
	StageType m_lastStage;

private:
	std::map<std::string, std::shared_ptr<ASTNode>> m_symbols;
	std::unique_ptr<AST::AST> m_ast = std::make_unique<AST::AST>();
	std::vector<ResultSection> m_resultSet;
};

} // namespace CoilCl
