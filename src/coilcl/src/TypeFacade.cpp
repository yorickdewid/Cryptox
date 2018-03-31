// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "TypeFacade.h"

using namespace CoilCl::AST;

void TypeFacade::Serialize(const TypeFacade& in, std::vector<uint8_t>& out)
{
	//
}

void TypeFacade::Deserialize(TypeFacade& out, const std::vector<uint8_t>& in)
{
	//
}

std::string TypeFacade::PointerName() const
{
	if (m_ptrCount == 0) {
		return "";
	}

	return " " + std::string(m_ptrCount, '*');
}
