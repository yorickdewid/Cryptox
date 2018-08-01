// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be
// copied and/or distributed without the express of the author.

#pragma once

#include "ASTNode.h"

#define BUILTIN_ROUTINE_IMPL(r) void static_##r(std::shared_ptr<BuiltinExpr>& builtinExpr)

namespace CoilCl
{
namespace BuiltinRoutine
{

BUILTIN_ROUTINE_IMPL(sizeof);
BUILTIN_ROUTINE_IMPL(static_assert);

} // namespace BuiltinRoutine
} // namespace CoilCl
