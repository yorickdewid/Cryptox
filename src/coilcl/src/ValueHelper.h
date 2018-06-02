// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Valuedef.h"

//#include <boost/any.hpp> //TODO: Why?
//#include <boost/variant.hpp>
//#include <boost/optional.hpp>
//#include <boost/lexical_cast.hpp>

//#include <string>

#ifdef CRY_DEBUG
#include <iostream>
#define DUMP_VALUE(v) \
	std::cout << v->DataType()->TypeName() << " >> " << v->Print() << std::endl;
#endif

#define CaptureValue(s) Util::CaptureValueRaw(std::move(s))

//TODO:
// - Void value
// - struct/union value
// - Cleanup old obsolete code

namespace CoilCl
{

// The value helper functions ease the creation and modification of the
// value objects as well as some functions to query specific properties.
// In any case should the helper functions be used instead of accessing
// the value objects directly. The helper functions are designed to support
// the most common value types as such that the caller does not need to
// cast the value into a specific type.

namespace Util
{

using namespace ::CoilCl::Valuedef;

//
// Create explicit value with automatic type
//

template<typename NativeType, typename ValueType> //TODO: FIXME: DEPRECATED
inline auto MakeValueObject(Typedef::BuiltinType&& type, ValueType value)
{
	return std::make_shared<ValueObject<NativeType>>(std::move(type), value);
}
template<typename Type = std::string> //TODO: FIXME: DEPRECATED
inline ValueType<Type> MakeString(Type v)
{
	return MakeValueObject<Type>(Typedef::BuiltinType::Specifier::CHAR, v);
}
template<typename Type = int> //TODO: FIXME: DEPRECATED
inline ValueType<Type> MakeInt(Type v)
{
	return MakeValueObject<Type>(Typedef::BuiltinType::Specifier::INT, std::move(v));
}
template<typename Type = float> //TODO: FIXME: DEPRECATED
inline ValueType<Type> MakeFloat(Type v)
{
	return MakeValueObject<Type>(Typedef::BuiltinType::Specifier::FLOAT, std::move(v));
}
template<typename Type = double> //TODO: FIXME: DEPRECATED
inline ValueType<Type> MakeDouble(Type v)
{
	return MakeValueObject<Type>(Typedef::BuiltinType::Specifier::DOUBLE, std::move(v));
}
template<typename Type = char> //TODO: FIXME: DEPRECATED
inline ValueType<Type> MakeChar(Type v)
{
	return MakeValueObject<Type>(Typedef::BuiltinType::Specifier::CHAR, v);
}
template<typename Type = bool> //TODO: FIXME: DEPRECATED
inline ValueType<Type> MakeBool(Type v)
{
	return MakeValueObject<Type>(Typedef::BuiltinType::Specifier::BOOL, v);
}
template<typename Type = void> //TODO: why?
inline ValueType<Type> MakeVoid()
{
	return std::make_shared<ValueObject<Type>>();
}

//
// Create explicit value with automatic type
// Version 2.0
//

inline auto MakeString2(const std::string& v)
{
	std::vector<char> ve(v.begin(), v.end());
	ve.shrink_to_fit();
	const auto builtin = MakeBuiltinType(Typedef::BuiltinType::Specifier::CHAR);
	return Value{ 0, AST::TypeFacade{ builtin }, Value::ValueVariant3{ std::move(ve) } };
}
inline auto MakeInt2(int v)
{
	const auto builtin = MakeBuiltinType(Typedef::BuiltinType::Specifier::INT);
	return Value{ 0, AST::TypeFacade{ builtin }, Value::ValueVariant2{ std::move(v) } };
}
inline auto MakeFloat2(float v)
{
	const auto builtin = MakeBuiltinType(Typedef::BuiltinType::Specifier::FLOAT);
	return Value{ 0, AST::TypeFacade{ builtin }, Value::ValueVariant2{ std::move(v) } };
}
inline auto MakeDouble2(double v)
{
	const auto builtin = MakeBuiltinType(Typedef::BuiltinType::Specifier::DOUBLE);
	return Value{ 0, AST::TypeFacade{ builtin }, Value::ValueVariant2{ std::move(v) } };
}
inline auto MakeChar2(char v)
{
	const auto builtin = MakeBuiltinType(Typedef::BuiltinType::Specifier::CHAR);
	return Value{ 0, AST::TypeFacade{ builtin }, Value::ValueVariant2{ std::move(v) } };
}
inline auto MakeBool2(bool v)
{
	const auto builtin = MakeBuiltinType(Typedef::BuiltinType::Specifier::BOOL);
	return Value{ 0, AST::TypeFacade{ builtin }, Value::ValueVariant2{ std::move(v) } };
}
inline auto MakePointer(Value&& v)
{
	return Value{ 0, AST::TypeFacade{ MakePointerType() }, std::move(v) };
}

//
// Create explicit array value with automatic type
//

inline auto MakeIntArray(int v[])
{
	const auto builtin = MakeBuiltinType(Typedef::BuiltinType::Specifier::INT);
	std::vector<int> ve(v, v + sizeof(v) / sizeof(v[0]));
	return Value{ 0, AST::TypeFacade{ builtin }, Value::ValueVariant3{ std::move(ve) } };
}
inline auto MakeIntArray(std::vector<int> v)
{
	const auto builtin = MakeBuiltinType(Typedef::BuiltinType::Specifier::INT);
	return Value{ 0, AST::TypeFacade{ builtin }, Value::ValueVariant3{ v } };
}
inline auto MakeFloatArray(std::vector<float> v)
{
	const auto builtin = MakeBuiltinType(Typedef::BuiltinType::Specifier::FLOAT);
	return Value{ 0, AST::TypeFacade{ builtin }, Value::ValueVariant3{ v } };
}
inline auto MakeDoubleArray(std::vector<double> v)
{
	const auto builtin = MakeBuiltinType(Typedef::BuiltinType::Specifier::DOUBLE);
	return Value{ 0, AST::TypeFacade{ builtin }, Value::ValueVariant3{ v } };
}
inline auto MakeBoolArray(std::vector<bool> v)
{
	const auto builtin = MakeBuiltinType(Typedef::BuiltinType::Specifier::BOOL);
	return Value{ 0, AST::TypeFacade{ builtin }, Value::ValueVariant3{ v } };
}

//
// Create implicit value with automatic type
//

template<typename NativeType>
inline Value CaptureValueRaw(NativeType&& v)
{
	return Detail::ValueDeductor{}(std::forward<NativeType>(v));
}

//
// Change value internals
//

// Create deep copy of value with the same contents
std::shared_ptr<Value> ValueCopy(const std::shared_ptr<Value>&); //TODO: rewrite

//
// Query value properties
//

// Evaluate the value as either true or false
bool EvaluateAsBoolean(std::shared_ptr<Value>);  //TODO: rename EvaluateValueAsBoolean
// Evaluate the value as an integer if possible
int EvaluateValueAsInteger(std::shared_ptr<Value>);
// Test if the value is array type
bool IsValueArray(std::shared_ptr<Value>);
// Check if value is set
bool IsValueInitialized(std::shared_ptr<Value>);

} // namespace Util
} // namespace CoilCl
