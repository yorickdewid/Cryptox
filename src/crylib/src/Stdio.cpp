// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Functional.h"

#include <cstdio>

//
// Corresponding header: stdio.h
//

namespace
{

CRY_METHOD(puts)
{
	const std::string str = ctx.GetParameter<std::string>("str");
	int result = puts(str.c_str());
	ctx.SetReturn(Util::MakeInt(result));
}

CRY_METHOD(putchar)
{
	const int chr = ctx.GetParameter<int>("c");
	int result = putchar(chr);
	ctx.SetReturn(Util::MakeInt(result));
}

CRY_METHOD(perror)
{
	const std::string str = ctx.GetParameter<std::string>("str");
	perror(str.c_str());
}

CRY_METHOD(remove)
{
	const std::string file = ctx.GetParameter<std::string>("file");
	int result = remove(file.c_str());
	ctx.SetReturn(Util::MakeInt(result));
}

CRY_METHOD(rename)
{
	const std::string oldname = ctx.GetParameter<std::string>("oldname");
	const std::string newname = ctx.GetParameter<std::string>("newname");
	int result = rename(oldname.c_str(), newname.c_str());
	ctx.SetReturn(Util::MakeInt(result));
}

CRY_METHOD(printf)
{
	const std::string fmt = ctx.GetParameter<std::string>("fmt");

	//TODO: ...
}

CRY_METHOD(putc)
{
	const int chr = ctx.GetParameter<int>("c");

	//TODO: ...
}

CRY_METHOD(getchar)
{
	int c = getchar();
	ctx.SetReturn(Util::MakeInt(c));
}

CRY_METHOD(scanf)
{
	ctx.GetParameter<std::string>("str");
	//GET_PARAMETER(0, std::string);

	//auto result = scanf(param0.c_str());
	//SET_RETURN(result);
}

} // namespace

namespace IO
{

std::list<EVM::ExternalMethod> RegisterFunctions()
{
	using namespace EVM;

	return {

		//
		// Output operations.
		//

		REGISTER_METHOD_PARAM("perror", perror, ParseSolidType("str", "s")),
		REGISTER_METHOD_PARAM("printf", printf, ParseSolidType("fmt", "s"), ParseSolidType("arg", "V")),
		REGISTER_METHOD_PARAM("putc", putc, ParseSolidType("c", "i")),
		REGISTER_METHOD_PARAM("putchar", putchar, ParseSolidType("c", "i")),
		REGISTER_METHOD_PARAM("puts", puts, ParseSolidType("str", "s")),

		//
		// Input operations.
		//

		REGISTER_METHOD("getchar", getchar),
		//REGISTER_METHOD_PARAM("scanf", scanf, ParseSolidType("str", "s")),

		//
		// File operations.
		//

		REGISTER_METHOD_PARAM("remove", remove, ParseSolidType("file", "s")),
		REGISTER_METHOD_PARAM("rename", rename, ParseSolidType("oldname", "s"), ParseSolidType("newname", "s")),
	};
}

} // namespace IO
