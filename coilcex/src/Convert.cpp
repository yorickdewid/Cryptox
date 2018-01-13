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

#define CONVERTER(c,o) { \
	c conv; \
	std::shared_ptr<AbstractConv> call = std::make_shared<CallableConv<decltype(conv)>>(GetPrioByConverterType(o), conv); \
	m_converters.push_back(std::move(call)); \
}

CryExe::Convert::Convert(std::vector<uint8_t>& inData, operations_type operations)
	: m_data{ inData }
{
	if (operations & Operations::CO_ENCRYPT) {
		// TODO
	}
	if (operations & Operations::CO_COMPRESS) {
		CONVERTER(CompressConv, Operations::CO_ENCRYPT);
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
	ConvertInvoke<std::greater>();
}

void CryExe::Convert::FromImage()
{
	ConvertInvoke<std::less>();
}
