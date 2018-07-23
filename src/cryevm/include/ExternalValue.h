// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "ExternalMethod.h"

#include "../../coilcl/src/ASTNode.h"
#include "../../coilcl/src/ValueHelper.h"

#include <string>
#include <cassert>

//#define PACKED_PARAM_DECL(s) SolidParameterFormat{}.Parse(s).Parameters()

namespace EVM
{

inline ExternalMethod::Parameter ParseSolidType(const std::string& paramName, const char str[])
{
	for (size_t i = 0; i < strlen(str); ++i) {
		assert(i < 100);
		switch (str[i]) {
		case 'i': {
			return ExternalMethod::Parameter{ paramName, Typedef::TypeFacade{ Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::INT) } };
		}
		case 's': {
			return ExternalMethod::Parameter{ paramName, Typedef::TypeFacade{ Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::CHAR) } };
		}
		case 'f': {
			return ExternalMethod::Parameter{ paramName, Typedef::TypeFacade{ Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::FLOAT) } };
		}
		case 'b': {
			return ExternalMethod::Parameter{ paramName, Typedef::TypeFacade{ Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::BOOL) } };
		}
		case 'd': {
			return ExternalMethod::Parameter{ paramName, Typedef::TypeFacade{ Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::DOUBLE) } };
		}
		case 'L': {
			return ExternalMethod::Parameter{ paramName, Typedef::TypeFacade{ Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::LONG) } };
		}
		case 'S': {
			return ExternalMethod::Parameter{ paramName, Typedef::TypeFacade{ Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::SHORT) } };
		}
		case 'U': {
			return ExternalMethod::Parameter{ paramName, Typedef::TypeFacade{ Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::UNSIGNED) } };
		}
		case 'G': {
			return ExternalMethod::Parameter{ paramName, Typedef::TypeFacade{ Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::SIGNED) } };
		}
		case 'v': {
			return ExternalMethod::Parameter{ paramName, Typedef::TypeFacade{ Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::VOID_T) } };
		}
		case 'V': {
			return ExternalMethod::Parameter{ paramName, Typedef::TypeFacade{ Util::MakeVariadicType() } };
		}
		}
	}

	throw 1; //TODO:
}

} // namespace EVM
