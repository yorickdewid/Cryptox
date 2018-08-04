// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/Converter.h>

namespace CryCC
{
namespace SubValue
{
namespace Conv
{

std::string Cast::PrintTag(Cast::Tag tag)
{
	switch (tag) {
	case Tag::INTEGRAL_CAST:
		return "integral cast";
	case Tag::ARRAY_CAST:
		return "array cast";
	case Tag::LTOR_CAST:
		return "ltor cast";
	case Tag::FUNCTION_CAST:
		return "function cast";
	}

	return "<unknown>";
}

} // namespace Conv
} // namespace SubValue
} // namespace CryCC
