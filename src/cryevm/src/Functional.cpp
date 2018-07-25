// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Functional.h"

#include <list>

namespace
{

CRY_METHOD(pause)
{
	CRY_UNUSED(ctx);

	/*const auto& value0 = ctx->ValueByIdentifier("__arg0__").lock();
	assert(value0);
	const auto param0 = value0->As<int>();

	ctx->CreateSpecialVar<RETURN_VALUE>(Util::MakeInt(param0));*/
}

CRY_METHOD(blub)
{
	int i = ctx.GetParameter<int>("i");
	int j = ctx.GetParameter<int>("j");
	i += j;
	ctx.SetReturn(Util::MakeInt(i));
}

} // namespace

namespace EVM
{

std::list<ExternalMethod> SymbolIndex()
{
	return {
		REGISTER_METHOD("pause", pause),
		REGISTER_METHOD_PARAM("blub", blub, ParseSolidType("i", "i"), ParseSolidType("j", "i")),
	};
}

} // namespace EVM
