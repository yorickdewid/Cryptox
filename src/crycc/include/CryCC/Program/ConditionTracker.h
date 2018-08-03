// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

namespace CryCC
{
namespace Program
{

class ConditionTracker
{
public:
	//TODO: Shiftable enum
	enum ProgramPhase
	{
		_INVAL = 0,             // Default invalid phase.

		CANONICAL = 100,        // Bootstrapping phase.
		DETECTION,              // Langauge detection phase.
		SUBSTITUTION,           // Token substitution phase.
		VALIDATION,             // Input validation phase.
		_PHASE1 = VALIDATION,   // End of phase 1

		STATIC_RESOLVED,        // Internal routines phase.
		ASSERTION_PASSED,       // Semantic assertion phase.
		_PHASE2 = ASSERTION_PASSED,   // End of phase 2

		COMPLIANT,              // Language compliant phase.
		OPTIMIZED,              // Object optimization phase.
		STRIPPED,               // Symbols are stripped phase.
		_PHASE3 = STRIPPED,     // End of phase 3

		_MAX = _PHASE3,         // Identify last phase
	};

	// Move condition into next phase. Each time a phase is
	// completed the condition is updated accordingly. Compiler
	// phases can only be moved forward. Phases can be skipped
	// but withint their own phase group.
	void Advance(ProgramPhase jump = _INVAL);

public:
	class Tracker
	{
		ConditionTracker& m_condition;

	public:
		// Initialize the tracker and set condition to initial phase.
		Tracker(const ConditionTracker& condition)
			: m_condition{ const_cast<ConditionTracker&>(condition) }
		{
			m_condition.Advance(CANONICAL);
		}

		// Jump to given phase.
		Tracker& Jump(ProgramPhase phase)
		{
			m_condition.Advance(phase);
			return (*this);
		}

		// Move condition into next phase.
		Tracker& operator++()
		{
			m_condition.Advance();
			return (*this);
		}
	};

public:
	//
	// Check program status.
	//

	inline auto IsInvalid() const noexcept { return m_treeCondition == _INVAL; }
	inline auto IsLanguage() const noexcept { return m_treeCondition >= _PHASE1; }
	inline auto IsRunnable() const noexcept { return m_treeCondition >= _PHASE2; }
	inline auto IsOptimized() const noexcept { return m_treeCondition >= _PHASE3; }
	inline auto IsAllPassed() const noexcept { return m_treeCondition == _MAX; }

private:
	ProgramPhase m_treeCondition{ _INVAL };
};

} // namespace Program
} // namespace CryCC
