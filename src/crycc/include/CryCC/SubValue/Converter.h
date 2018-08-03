// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/SubValue/TypeFacade.h>

#include <Cry/Cry.h>

#include <stdexcept>
#include <sstream>

namespace CryCC
{
namespace SubValue
{
namespace Conv
{

//template<typename _Ty1, typename Ty2>
//struct is_convertible : std::false_type { };
//
//template<typename _Ty1>
//struct is_convertible<_Ty1, _Ty1> : std::true_type {};
//
//template<>
//struct is_convertible<Typedef::RecordType, Typedef::RecordType> : std::false_type {};
//
//template<>
//struct is_convertible<Typedef::VariadicType, Typedef::VariadicType> : std::false_type {};

class ConverterException : public std::exception
{
public:
	ConverterException() noexcept = default;

	//explicit ConverterException(/*, int line, int column*/) noexcept
	//{
	//	std::stringstream ss;
	//	ss << "cannot initialize type 'x' with entity of type 'y'";
	//	_msg = ss.str();
	//}

	virtual const char *what() const noexcept
	{
		return _msg.c_str();
	}

protected:
	std::string _msg;
};

class MutateOperation
{
};

// Convert from one integer to another integer value.
class IntegralCast : public MutateOperation
{
};

// Convert from one floating point to another floating point value.
class FloatingCast : public MutateOperation
{
};

class IntegralToFloatingCast : public MutateOperation
{
};

class FloatingToIntegralCast : public MutateOperation
{
};

class Cast
{
public:
	enum class Tag
	{
		NONE_CAST,        // Do not cast anything.
		INTEGRAL_CAST,    // Cast Integer to integer.
		FLOATING_CAST,    // Cast float to float point.
		ARRAY_CAST,       // 
		LTOR_CAST,        // Cast left to right value.
		CONST_CAST,       // Cast to or from constant.
		FUNCTION_CAST,    //
	};

public:
	static Tag Transmute(const Typedef::TypeFacade& baseType, const Typedef::TypeFacade& initType)
	{
		CRY_UNUSED(baseType);
		CRY_UNUSED(initType);

		//TODO
		return Tag::NONE_CAST;
	}

	static std::string PrintTag(Cast::Tag);
};

} // namespace Conv
} // namespace SubValue
} // namespace CryCC
