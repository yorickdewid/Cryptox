// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Functional.h"
#include "ExternalMethod.h"

#include <list>

namespace
{

CRY_METHOD(pause)
{
	// TODO...
}

} // namespace

namespace EVM
{

std::list<ExternalMethod> SymbolIndex()
{
	return { ExternalMethod{ "pause", &cry_pause, /*PACKED_PARAM_DECL("s")*/{} }, };
}

} // namespace EVM
