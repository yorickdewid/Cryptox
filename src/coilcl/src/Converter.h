// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <stdexcept>
#include <sstream>

#include "Typedef.h"

namespace CoilCl
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

class IntegralCast : public MutateOperation
{
};

class Cast
{
public:
	enum class Tag
	{
		INTEGRAL_CAST,
		ARRAY_CAST,
		LTOR_CAST,
		CONST_CAST,
		FUNCTION_CAST,
	};

public:
	template<typename _ConvTy>
	static Tag Transmute(_ConvTy from, _ConvTy to)
	{
		//TODO
		return Tag::INTEGRAL_CAST;
	}

	static std::string PrintTag(Cast::Tag);
};

} // namespace Conv
} // namespace CoilCl
