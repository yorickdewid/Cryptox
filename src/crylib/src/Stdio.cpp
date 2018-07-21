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

CRY_METHOD(perror)
{
	//perror(str);
}

CRY_METHOD(remove)
{
	//remove(file);
}

} // namespace

namespace IO
{

//TODO:
std::list<EVM::ExternalMethod> RegisterFunctions()
{
	using namespace EVM;

	return {
		ExternalMethod{ "puts", &cry_puts, /*PACKED_PARAM_DECL("s")*/ {} },
		ExternalMethod{ "perror", &cry_perror, /*PACKED_PARAM_DECL("s")*/ {} },
		ExternalMethod{ "remove", &cry_remove, /*PACKED_PARAM_DECL("q")*/ {} },
	};
}

} // namespace IO
