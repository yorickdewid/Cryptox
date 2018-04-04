// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Planner.h"
#include "NoStrat.h"
#include "Interpreter.h"

using namespace EVM;

Planner::Planner(ProgramPtr&& program, Plan plan)
	: m_program{ std::move(program) }
	, m_opt{ plan }
{
}

template<typename _Ty = None, typename _ObjTy>
std::unique_ptr<Strategy> YieldStrategy(_ObjTy planner)
{
	return std::make_unique<_Ty>(*planner);
}

std::unique_ptr<Strategy> Planner::DetermineStrategy()
{
	// Algoritm for choosing the 'best' runner for the program.
	// The planner takes the architectural options, pereferences
	// and program structure into account when making a strategy
	// determination. Runners can be non-executable and the caller
	// *must* check this condition preparatory to invokation.
	/*
	if (wants_native && plan != NO_ARCH) {
		if (has_native_code(x64)) {
			return YieldStrategy<OSNative>();
		}
		if (can_convert_to_native(x64)) {
			convert_native(x64)
			return YieldStrategy<OSNative>();
			if (test_native_fallback(x86)) {
				convert_native(x86)
				return YieldStrategy<OSNative>();
			}
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
	
	return YieldStrategy<Interpreter>(this);
}
