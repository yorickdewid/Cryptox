// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Functional.h"

// Functional routines.
//
// The functional unit is the designated place for any external
// routines which are not part of any language standard, but are
// universal enough to incorporate them in the module. The methods
// behave in the same manner as an external method would.
//
// NOTE: The anonymous namespace must inhibit all the external
//       methods. Any new methods *must* be appended to this namespace.
//       The methods itself are unrelated to any of the other methods.
//
// NOTE: External methods must me registered in the external method
//       registry down at the bottom. Registered methods become
//       automatically available to all the language environments.

namespace
{

// The pause method.
// CALL:    pause()
CRY_METHOD(pause)
{
	CRY_UNUSED(ctx);
}

// The blub method.
// CALL:    blub(i -> int, j -> int)
// RETURN:  int
CRY_METHOD(blub)
{
	int i = Util::ValueCastNative<int>((*ctx.GetParameter("i")));
	int j = Util::ValueCastNative<int>(*ctx.GetParameter("j"));
	i += j;
	ctx.SetReturn(Util::MakeInt(i));
}

// Heap allocation method.
// CALL:    heap_alloc(size -> int)
CRY_METHOD(heap_alloc)
{
	unsigned int size = Util::ValueCastNative<int>(*ctx.GetParameter("size"));
	CRY_UNUSED(size);
}

// Heap free method.
// CALL:    heap_alloc(ptr -> int)
CRY_METHOD(heap_free)
{
	intptr_t ptr = Util::ValueCastNative<int>(*ctx.GetParameter("ptr"));
	CRY_UNUSED(ptr);
}

} // namespace

namespace EVM
{

// Register external methods in the registry below. Each 
// of the entries is supposted to have:
//
//   1.) A method name (required).
//   2.) Link to the method routine (required).
//   3.) Variable number of parameter pairs.
ExternalMethodRegistry SymbolIndex()
{
	return {
		REGISTER_METHOD("pause", pause),
		REGISTER_METHOD_PARAM("blub", blub, ParseSolidType("i", "i"), ParseSolidType("j", "i")),
		REGISTER_METHOD_PARAM("heap_alloc", heap_alloc, ParseSolidType("size", "i")),
		REGISTER_METHOD_PARAM("heap_free", heap_free, ParseSolidType("ptr", "i")),
	};
}

} // namespace EVM
