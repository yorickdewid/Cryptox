// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "State.h"

namespace EVM
{
namespace GlobalExecutionState
{

static std::list<ExternalMethod> g_externalSymbolList;

void Set(const std::list<ExternalMethod>& symbolList)
{
	g_externalSymbolList = symbolList;
}

void UnsetAll()
{
	g_externalSymbolList.clear();
}

// FUTURE: logarithmic search or static search
// FUTURE: memory priority cache
const ExternalMethod *FindExternalSymbol(const std::string& symbol)
{
	auto it = std::find_if(g_externalSymbolList.cbegin(), g_externalSymbolList.cend(), [&](const ExternalMethod& method) {
		return method.symbol == symbol;
	});
	if (it == g_externalSymbolList.cend()) { return nullptr; }
	return &(*it);
}


} // namespace GlobalExecutionState
} // namespace EVM
