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
}

CRY_METHOD(blub)
{
	int i = ctx.GetParameter<int>("i");
	int j = ctx.GetParameter<int>("j");
	i += j;
	ctx.SetReturn(Util::MakeInt(i));
}

CRY_METHOD(heap_alloc)
{
	unsigned int size = ctx.GetParameter<int>("size");
}

CRY_METHOD(heap_free)
{
	intptr_t ptr = ctx.GetParameter<int>("ptr");
}

} // namespace

namespace EVM
{

std::list<ExternalMethod> SymbolIndex()
{
	return {
		REGISTER_METHOD("pause", pause),
		REGISTER_METHOD_PARAM("blub", blub, ParseSolidType("i", "i"), ParseSolidType("j", "i")),
		REGISTER_METHOD_PARAM("heap_alloc", heap_alloc, ParseSolidType("size", "i")),
		REGISTER_METHOD_PARAM("heap_free", heap_free, ParseSolidType("ptr", "i")),
	};
}

} // namespace EVM
