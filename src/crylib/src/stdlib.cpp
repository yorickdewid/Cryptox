// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Functional.h"

#include <cstdlib>

namespace
{

CRY_METHOD(abs)
{
	const int i = ctx.GetParameter<int>("i");
	int result = abs(i);
	ctx.SetReturn(Util::MakeInt(result));
}

CRY_METHOD(system)
{
    const std::string command = ctx.GetParameter<std::string>("command");
	auto result = system(command.c_str());
	ctx.SetReturn(Util::MakeInt(result));
}

} // namespace

namespace Lib
{

std::list<EVM::ExternalMethod> RegisterFunctions()
{
	using namespace EVM;

	return {
		REGISTER_METHOD_PARAM("abs", abs, ParseSolidType("i", "i")),
        REGISTER_METHOD_PARAM("system", system, ParseSolidType("command", "s")),
	};
}

} // namespace Lib
