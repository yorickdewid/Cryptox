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
#include "ConditionTracker.h"

#include <Cry/Cry.h>
#include <Cry/Serialize.h>

#include <map>
#include <vector>
#include <memory>

#define CHECK_LOCK() if (m_locked) { throw AccessViolationException{}; }

namespace CoilCl
{

// Program is the resulting structure for all compiler stages.
// Only stages are allowed to alter the program internals. The
// program is passed to a program runner in which case the program
// is run, or the program structure is dissected in an analyzer.
class Program final
{
public:
	class ResultSection
	{
		using SizeType = size_t;
		using BufferType = Cry::ByteArray;

	private:
		BufferType m_content;

	public:
		enum Tag
		{
			AIIPX,         // Resulting section for AIIPX content.
			CASM,          // Resulting section for CASM content.
			NATIVE,        // Resulting section for native content.
			COMPLEMENTARY, // Resulting section for additional content.
		} m_tag;

	public:
		ResultSection(Tag tag = Tag::COMPLEMENTARY)
			: m_tag{ tag }
		{
		}

		// Get size of section content.
		inline SizeType Size() const noexcept { return m_content.size(); }
		// Get context object.
		inline BufferType& Data() noexcept { return m_content; }
	};

	struct AccessViolationException : public std::exception
	{
	};

public:
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

	//
	// Disable assignment operators.
	//

	Program& operator=(const Program&) = delete;
	Program& operator=(Program&&) = delete;

	//
	// AST operations.
	//

	// Get reference to the AST tree.
	inline auto Ast() { /*CHECK_LOCK();*/ return m_ast->tree_ref(); }
	// Access internal AST tree indirect.
	inline auto AstPassthrough() const { return m_ast->operator->(); }

	// Symbol operations
	void PrintSymbols();
	bool MatchSymbol(const std::string&);
	inline bool HasSymbols() const { return !m_symbols.empty(); }
	inline bool HasSymbol(const std::string& name) const { return m_symbols.find(name) != m_symbols.end(); }
	auto& FillSymbols() { CHECK_LOCK(); return m_symbols; } //TODO: friend?

	// TODO return const if locked
	// Get memory block.
	ResultSection& GetResultSection(ResultSection::Tag tag = ResultSection::Tag::COMPLEMENTARY);

	// Retieve program condition.
	inline const ConditionTracker& Condition() const { return m_treeCondition; }

	operator bool() const noexcept { return !!m_ast; }

	// Lock the program and throw on modifier methods,
	void Lock() { m_locked = true; }

	template<typename... ArgTypes>
	static void Bind(std::unique_ptr<Program>&& program, ArgTypes&&... args)
	{
		auto ptr = program.release();
		program = std::make_unique<Program>(std::move(*(ptr)), std::forward<ArgTypes>(args)...);
		delete ptr;
	}

	template<typename... ArgTypes>
	static std::unique_ptr<Program> MakeProgram(ArgTypes&&... args)
	{
		return std::make_unique<Program>(std::forward<ArgTypes>(args)...);
	}

private:
	ConditionTracker m_treeCondition;
	StageType m_lastStage;
	bool m_locked{ false };

private:
	std::map<std::string, std::shared_ptr<CoilCl::AST::ASTNode>> m_symbols;
	std::unique_ptr<AST::AST> m_ast = std::make_unique<AST::AST>();
	std::vector<ResultSection> m_resultSet;
};

} // namespace CoilCl
