// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Planner.h"
#include "Interpreter.h"

using namespace EVM;

Planner::Planner(ProgramPtr&& program, Plan plan)
	: m_program{ std::move(program) }
	, m_opt{ plan }
{
}

template<typename _Ty = Strategy>
Strategy YieldStrategy()
{
	return _Ty{};
}

Strategy Planner::DetermineStrategy()
{
	// Algoritm
	/*
	if (wants_native && plan != NO_ARCH) {
		if (has_native_code(x64)) {
			return YieldStrategy<OSNative>();
		}
		if (can_convert_to_native(x64)) {
			convert_native()
			return YieldStrategy<OSNative>();
		}
		if (plan == NATIVE_ONLY) {
			return YieldStrategy<>();
		}
	}
	if (has_casm && plan == ALL) {
		return YieldStrategy<VirtualMachine>();
	}
	if (wants_casm && plan == ALL) {
		if (can_convert_to_casm) {
			convert_casm()
			return YieldStrategy<VirtualMachine>();
		}
	}
	if (plan != NATIVE_ONLY) {
		return YieldStrategy<Interpreter>();
	}
	return YieldStrategy<>();
	*/
	
	return YieldStrategy<Interpreter>();
}
