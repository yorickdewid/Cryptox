// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Functional.h"

#include <cstdio>

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
	const int chr = ctx.GetParameter<int>("str");
	int result = putchar(chr);
	ctx.SetReturn(Util::MakeInt(result));
}

CRY_METHOD(perror)
{
	//perror(str);
}

CRY_METHOD(remove)
{
	//remove(file);
}

CRY_METHOD(printf)
{
	/*GET_PARAMETER(0, std::string);
	GET_VA_PARAMETER(0, int);

	auto result = printf(param0.c_str(), va_param0);
	SET_RETURN(result);*/
}

CRY_METHOD(scanf)
{
	//GET_PARAMETER(0, std::string);

	//auto result = scanf(param0.c_str());
	//SET_RETURN(result);
}

CRY_METHOD(system)
{
	/*GET_PARAMETER(0, std::string);
	auto result = system(param0.c_str());
	SET_RETURN(result);*/
}

} // namespace

namespace IO
{

std::list<EVM::ExternalMethod> RegisterFunctions()
{
	using namespace EVM;

	return {
		REGISTER_METHOD_PARAM("puts", puts, ParseSolidType("str", "s")),
		REGISTER_METHOD_PARAM("putchar", putchar, ParseSolidType("c", "i")),
		//ExternalMethod{ "putchar", &cry_putchar, /*PACKED_PARAM_DECL("i")*/ {} },
		//ExternalMethod{ "puts", &cry_puts, /*PACKED_PARAM_DECL("s")*/ {} },
		//ExternalMethod{ "perror", &cry_perror, /*PACKED_PARAM_DECL("s")*/ {} },
		//ExternalMethod{ "remove", &cry_remove, /*PACKED_PARAM_DECL("q")*/ {} },
		//ExternalMethod{ "printf", &cry_printf, /*PACKED_PARAM_DECL("sV")*/ {} },
		//ExternalMethod{ "scanf", &cry_scanf, /*PACKED_PARAM_DECL("sV")*/ {} },
		//ExternalMethod{ "system", &cry_system, /*PACKED_PARAM_DECL("s")*/ {} },
	};
}

} // namespace IO
