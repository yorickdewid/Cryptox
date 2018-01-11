// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Convert.h"
#include "Compress.h"

#include <memory>
#include <algorithm>
#include <functional>
#include <cassert>

CryExe::Convert::Convert(std::vector<uint8_t>& inData, operations_type operations)
	: m_data{ inData }
{
	/*if (operations & Operations::CO_ENCRYPT) {
		m_converters.push_back(CallableConv<>{ GetPrioByConverterType(Operations::CO_ENCRYPT) });
	}*/
	if (operations & Operations::CO_COMPRESS) {
		CompressConv conv;
		std::shared_ptr<AbstractConv> call = std::make_shared<CallableConv<CompressConv>>(GetPrioByConverterType(Operations::CO_ENCRYPT), conv);
		m_converters.push_back(std::move(call));
	}
}

int CryExe::Convert::GetPrioByConverterType(Operations operation)
{
	switch (operation) {
	case Operations::CO_ENCRYPT:
		return (1 << 0);
	case Operations::CO_COMPRESS:
		return (1 << 3);
	default:
		break;
	}

	assert(0);
	return -1;
}

void CryExe::Convert::ToImage()
{
	while (!m_converters.empty()) {
		auto conv = NextConverter<std::greater>();
		conv->Call(m_data);
		m_converters.pop_front();
	}
}

void CryExe::Convert::FromImage()
{
	while (!m_converters.empty()) {
		auto conv = NextConverter<std::less>();
		conv->Call(m_data);
		m_converters.pop_front();
	}
}
