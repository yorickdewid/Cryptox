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
	out.push_back(static_cast<uint8_t>(typePack.size()));
	out.insert(out.cend(), typePack.begin(), typePack.end());
	out.push_back(static_cast<uint8_t>(in.PointerCount()));

	// Typedef base generic options
	out.push_back(static_cast<uint8_t>(in->IsInline()));
	out.push_back(static_cast<uint8_t>(in->StorageClass()));
	out.push_back(static_cast<uint8_t>(in->TypeQualifiers()[0]));
	out.push_back(static_cast<uint8_t>(in->TypeQualifiers()[1]));
}

// Deserialize byte stream into type
void TypeFacade::Deserialize(TypeFacade& out, const std::vector<uint8_t>& in)
{
	using Typedef::BuiltinType;

	assert(in.size() > 5);
	int offset = 0;
	const auto sizeOfEnvelope = static_cast<size_t>(in.at(offset++));
	out.SetPointer(static_cast<size_t>(in.at(offset++)));

	// Typedef base generic options
	if (static_cast<bool>(in.at(offset++))) {
		out->SetInline();
	}
	auto qq = static_cast<BuiltinType::StorageClassSpecifier>(in.at(offset++));
	out->SetStorageClass(qq);
	out->SetQualifier(static_cast<BuiltinType::TypeQualifier>(in.at(offset++)));
	out->SetQualifier(static_cast<BuiltinType::TypeQualifier>(in.at(offset++)));
}

std::string TypeFacade::PointerName() const
{
	if (m_ptrCount == 0) {
		return "";
	}

	return " " + std::string(m_ptrCount, '*');
}
