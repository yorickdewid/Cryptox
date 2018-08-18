// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be
// copied and/or distributed without the express of the author.

#pragma once

// Project includes.
#include <CryCC/AST.h>

#define BUILTIN_ROUTINE_IMPL(r) void static_##r(std::shared_ptr<CryCC::AST::BuiltinExpr>& builtinExpr)

namespace CoilCl
{
namespace BuiltinRoutine
{

// Any throw of the 'std::exception' type will be caught and wrapped
// in this exception class. The exception is then thrown up the chain.
class Exception : public std::runtime_error
{
public:
	Exception(const std::string& routineName, const std::string& what)
		: std::runtime_error{ routineName + ": " + what }
	{
	}
};

BUILTIN_ROUTINE_IMPL(sizeof);
BUILTIN_ROUTINE_IMPL(static_assert);

} // namespace BuiltinRoutine
} // namespace CoilCl
