// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <string>
#include <functional>

#define CRY_METHOD(c) void cry_##c(EVM::Context::Function& ctx)

#define GET_PARAMETER(i,c) \
	const auto& value##i = ctx->ValueByIdentifier("__arg" #i "__").lock(); \
	assert(value##i); \
	const auto param##i = value##i->As<c>();

#define GET_VA_PARAMETER(i,c) \
	const auto& va_value##i = ctx->ValueByIdentifier("__va_arg" #i "__").lock(); \
	assert(va_value##i); \
	const auto va_param##i = va_value##i->As<c>();

#define SET_RETURN(r) ctx->CreateSpecialVar<RETURN_VALUE>(Util::MakeInt(r));

namespace EVM
{

class AbstractContext;
class GlobalContext;
class UnitContext;
class CompoundContext;
class FunctionContext;

namespace Context
{

using Global = std::shared_ptr<GlobalContext>;
using Unit = std::shared_ptr<UnitContext>;
using Compound = std::shared_ptr<CompoundContext>;
using Function = std::shared_ptr<FunctionContext>;

using WeakGlobal = std::weak_ptr<GlobalContext>;
using WeakUnit = std::weak_ptr<UnitContext>;
using WeakCompound = std::weak_ptr<CompoundContext>;
using WeakFunction = std::weak_ptr<FunctionContext>;

} // namespace Context

struct ExternalMethod
{
	const std::string symbol;
	const std::function<void(Context::Function&)> functional;
	const std::shared_ptr<ParamStmt> params;

	bool HasParameters() const noexcept { return params != nullptr; }

	ExternalMethod(const std::string symbol, std::function<void(Context::Function&)> func, std::shared_ptr<ParamStmt> params = {})
		: symbol{ symbol }
		, functional{ func }
		, params{ params }
	{
	}
};

} // namespace EVM
