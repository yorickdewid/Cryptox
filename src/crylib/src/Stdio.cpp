// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Functions.h"

#include <cstdio>

namespace
{

CRY_METHOD(puts)
{
	/*GET_PARAMETER(0, std::string);

	auto result = puts(param0.c_str());
	SET_RETURN(result);*/
}

CRY_METHOD(putchar)
{
	/*GET_PARAMETER(0, int);

	auto result = putchar(param0);
	SET_RETURN(result);*/
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

CRY_METHOD(error)
{
	//const auto& value = GET_DEFAULT_ARG(0);
	//const auto& value2 = GET_VA_LIST_ARG(0);
	//assert(value);
	//assert(value2);
	//const auto arg0 = value->As<int>();
	//const auto arg1 = value->As<std::string>();
	//throw arg0; //TODO: or something
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
		ExternalMethod{ "puts", &cry_puts, /*PACKED_PARAM_DECL("s")*/ {} },
		ExternalMethod{ "putchar", &cry_putchar, /*PACKED_PARAM_DECL("i")*/ {} },
		ExternalMethod{ "puts", &cry_puts, /*PACKED_PARAM_DECL("s")*/ {} },
		ExternalMethod{ "perror", &cry_perror, /*PACKED_PARAM_DECL("s")*/ {} },
		ExternalMethod{ "remove", &cry_remove, /*PACKED_PARAM_DECL("q")*/ {} },
		ExternalMethod{ "printf", &cry_printf, /*PACKED_PARAM_DECL("sV")*/ {} },
		ExternalMethod{ "scanf", &cry_scanf, /*PACKED_PARAM_DECL("sV")*/ {} },
		ExternalMethod{ "error", &cry_error, /*PACKED_PARAM_DECL("is")*/ {} },
		ExternalMethod{ "system", &cry_system, /*PACKED_PARAM_DECL("s")*/ {} },
	};
}

} // namespace IO
