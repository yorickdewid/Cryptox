// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/AST/AST.h>

#include <CryCC/Program/ConditionTracker.h>
#include <CryCC/Program/Stage.h>
#include <CryCC/Program/Type.h>
#include <CryCC/Program/Result.h>
#include <CryCC/Program/Symbol.h>

#include <Cry/Cry.h>
#include <Cry/Serialize.h>

#include <map>
#include <vector>
#include <memory>

#define CHECK_LOCK() if (m_locked) { throw AccessViolationException{}; }

namespace CryCC
{
namespace Program
{

// Program is the resulting structure for all compiler stages.
// Only stages are allowed to alter the program internals. The
// program is passed to a program runner in which case the program
// is run, or the program structure is dissected in an analyzer.
class Program final
{
	struct AccessViolationException : public std::exception
	{
	};

public:
	Program() = default;
	Program(const Program&) = delete;
	Program(Program&&) = default;

	// Move AST into program.
	Program(AST::AST&&);

	// OBSOLETE, REMOVE, TODO, FIXME
	// Create program from other program and unit tree.
	Program(Program&&, AST::ASTNodeType&&);

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

	//
	// Symbol operations.
	//

	inline bool HasSymbols() const noexcept { return !m_symbols.Empty(); }
	inline bool SymbolCount() const noexcept { return m_symbols.Count(); }
	SymbolMap& SymbolTable() { CHECK_LOCK(); return m_symbols; }
	const SymbolMap& StaticSymbolTable() const { return m_symbols; }

	//
	// Resultset operations.
	//

	//TODO: lock
	// Request or allocate a program owned result set.
	//
	// The program structure holds a map with an infinite number of
	// unallocated result set slots. Once a slot is requested by the
	// caller, the query will yield and existing result set object, or
	// a new result set is instantiated. The additional parameters in
	// function call serve as constructor parameters and will be 
	// forwarded down to the object. The object must implement the
	// interface or the compiler will ignore the routine.
	template<typename SetType, ResultInterface::slot_type Slot, typename... ArgTypes>
	ResultInterface& ResultSectionSlot(ArgTypes&&... args)
	{
		static_assert(std::is_base_of<ResultInterface, SetType>::value, "must inherit ResultInterface");

		// Query map for existing set in this slot.
		const auto& itExisting = m_resultSet.find(Slot);
		if (itExisting != m_resultSet.end()) {
			return (*itExisting->second);
		}

		// Allocate a new object in this slot and return it to the caller.
		const auto& itNew = m_resultSet.emplace(Slot, std::make_unique<SetType>(std::forward<ArgTypes>(args)...)).first;
		return (*itNew->second);
	}

	// Release a resultset from the slot position it occupies. This
	// call is a no-op when the slot is unallocated.
	template<ResultInterface::slot_type Slot>
	void ResultSectionSlotRelease()
	{
		const auto& it = m_resultSet.find(Slot);
		if (it == m_resultSet.end()) { return; }
		m_resultSet.erase(Slot);
	}

	// Retieve program condition.
	inline const ConditionTracker& Condition() const { return m_treeCondition; }
	// Test if a tree is set.
	operator bool() const noexcept { return !!m_ast; }

	//
	// Access operations.
	//

	// Lock the program and throw on modifier methods.
	void Lock() { m_locked = true; }
	// Check if program is in locked mode.
	bool IsLocked() const noexcept { return m_locked; }

	// Bind a tree struture to program, but only once.
	template<typename... ArgTypes>
	static void Bind(ProgramType& program, ArgTypes&&... args)
	{
		assert(!program->m_ast);
		program->m_ast = std::make_unique<AST::AST>(std::forward<ArgTypes>(args)...);
	}

	//FUTURE: Health check

private:
	ConditionTracker m_treeCondition;
	StageType m_lastStage;
	bool m_locked{ false };

private:
	SymbolMap m_symbols;
	std::unique_ptr<AST::AST> m_ast{ nullptr }; //TODO: Point to an ASTNode directly
	std::map<ResultInterface::slot_type, std::unique_ptr<ResultInterface>> m_resultSet;
};

} // namespace Program
} // namespace CryCC
