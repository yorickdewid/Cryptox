// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/Program/ConditionTracker.h>

#include <Cry/Cry.h>

#include <iostream>

#ifdef CRY_DEBUG
# define CRY_DEBUG_TRACE CRY_DEBUG_TRACE_ALL || 1
#endif

namespace
{

using namespace CryCC::Program;

template<int Increment = 1>
void PhaseIncrement(ConditionTracker::ProgramPhase& phase)
{
	phase = static_cast<ConditionTracker::ProgramPhase>(static_cast<int>(phase) + Increment);
}

template<int Decrement = 1>
void PhaseDecrement(ConditionTracker::ProgramPhase& phase)
{
	phase = static_cast<ConditionTracker::ProgramPhase>(static_cast<int>(phase) - Decrement);
}

#ifdef CRY_DEBUG_TRACE

const char *PhaseName(ConditionTracker::ProgramPhase phase)
{
	switch (phase)
	{
	case ConditionTracker::CANONICAL:
		return "Canonical";
	case ConditionTracker::DETECTION:
		return "Detection";
	case ConditionTracker::SUBSTITUTION:
		return "Substitution";
	case ConditionTracker::VALIDATION:
		return "Validation";
	case ConditionTracker::STATIC_RESOLVED:
		return "Static Resolved";
	case ConditionTracker::ASSERTION_PASSED:
		return "Assertion Passed";
	case ConditionTracker::COMPLIANT:
		return "Compliant";
	case ConditionTracker::OPTIMIZED:
		return "Optimized";
	case ConditionTracker::STRIPPED:
		return "Stripped";
	}

	return "Invalid";
}

#endif

} // namespace

namespace CryCC
{
namespace Program
{

// Advance or jump the program phase.
void ConditionTracker::Advance(ProgramPhase jump)
{
	if (m_treeCondition == ConditionTracker::ProgramPhase::_MAX) {
		return;
	}

	if (jump == _INVAL) {
		PhaseIncrement(m_treeCondition);
	}
	else if (jump > m_treeCondition) {
		m_treeCondition = jump;
	}

#ifdef CRY_DEBUG_TRACE
	std::cout << "phase " << PhaseName(m_treeCondition) << " done" << std::endl;
#endif
}

} // namespace Program
} // namespace CryCC
