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

// Serialize type into byte stream
void TypeFacade::Serialize(const TypeFacade& in, std::vector<uint8_t>& out)
{
	using Typedef::BuiltinType;

	const auto typePack = in->TypeEnvelope();
	out.reserve(typePack.size());
	out.push_back(static_cast<uint8_t>(in.PointerCount()));
	out.push_back(static_cast<uint8_t>(typePack.size()));
	out.insert(out.cend(), typePack.begin(), typePack.end());
	out.shrink_to_fit();
}

// Deserialize byte stream into type
void TypeFacade::Deserialize(TypeFacade& out, const std::vector<uint8_t>& in)
{
	using Typedef::BuiltinType;

	auto ptrCount = static_cast<size_t>(in.at(0));
	const auto sizeOfEnvelope = static_cast<size_t>(in.at(1));
	assert(sizeOfEnvelope + 2 == in.size());

	//FUTURE: Replace copy by move
	std::vector<uint8_t> type;
	type.resize(in.size() - 2);
	std::copy(in.begin() + 2, in.cend(), type.begin());
	Typedef::ValueType ptr = Util::MakeType(std::move(type));
	
	// Set type facade options
	TypeFacade tmp{ ptr };
	tmp.SetPointer(ptrCount);
	out = tmp;
}

std::string TypeFacade::PointerName() const
{
	if (m_ptrCount == 0) {
		return "";
	}

	return " " + std::string(m_ptrCount, '*');
}
