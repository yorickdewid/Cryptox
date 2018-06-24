// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Valuedef.h"

#ifdef CRY_DEBUG
#include <iostream>
#define DUMP_VALUE(v) \
	std::cout << v.Type()->TypeName() << " >> " << v.Print() << std::endl;
#endif

#define CaptureValue(s) Util::CaptureValueRaw(std::move(s))

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

using namespace Valuedef;
using namespace Typedef;

//
// Create explicit value with automatic type
//

inline auto MakeString(const std::string& v)
{
	std::vector<char> ve(v.begin(), v.end());
	ve.shrink_to_fit();
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::CHAR);
	return Value{ 0, TypeFacade{ builtin }, Value::ValueVariant3{ std::move(ve) } };
}
inline auto MakeInt(int v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::INT);
	return Value{ 0, TypeFacade{ builtin }, Value::ValueVariant2{ std::move(v) } };
}
inline auto MakeFloat(float v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::FLOAT);
	return Value{ 0, TypeFacade{ builtin }, Value::ValueVariant2{ std::move(v) } };
}
inline auto MakeDouble(double v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::DOUBLE);
	return Value{ 0, TypeFacade{ builtin }, Value::ValueVariant2{ std::move(v) } };
}
inline auto MakeChar(char v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::CHAR);
	return Value{ 0, TypeFacade{ builtin }, Value::ValueVariant2{ std::move(v) } };
}
inline auto MakeBool(bool v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::BOOL);
	return Value{ 0, TypeFacade{ builtin }, Value::ValueVariant2{ std::move(v) } };
}

//
// Create explicit array value with automatic type
//

inline auto MakeIntArray(int v[])
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::INT);
	std::vector<int> ve(v, v + sizeof(v) / sizeof(v[0]));
	return Value{ 0, TypeFacade{ builtin }, Value::ValueVariant3{ std::move(ve) } };
}
inline auto MakeIntArray(std::vector<int> v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::INT);
	return Value{ 0, TypeFacade{ builtin }, Value::ValueVariant3{ v } };
}
inline auto MakeFloatArray(std::vector<float> v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::FLOAT);
	return Value{ 0, TypeFacade{ builtin }, Value::ValueVariant3{ v } };
}
inline auto MakeDoubleArray(std::vector<double> v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::DOUBLE);
	return Value{ 0, TypeFacade{ builtin }, Value::ValueVariant3{ v } };
}
inline auto MakeBoolArray(std::vector<bool> v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::BOOL);
	return Value{ 0, TypeFacade{ builtin }, Value::ValueVariant3{ v } };
}

//
// Create pointer to another value
//

inline auto MakePointer(Value&& v)
{
	return Value{ 0, TypeFacade{ MakePointerType() }, std::move(v) };
}

//
// Create explicit record value with automatic type
//

inline auto MakeStruct(RecordValue&& v, const std::string structName = {})
{
	const std::string name = v.HasRecordName() ? v.RecordName() : structName;
	return Value{ 0, TypeFacade{ MakeRecordType(name, RecordType::Specifier::STRUCT) }, std::move(v) };
}
inline auto MakeUnion(RecordValue&& v, const std::string structName = {})
{
	const std::string name = v.HasRecordName() ? v.RecordName() : structName;
	return Value{ 0, TypeFacade{ MakeRecordType(name, RecordType::Specifier::UNION) }, std::move(v) };
}

//
// Create implicit value with automatic type
//

template<typename NativeType>
inline Value CaptureValueRaw(NativeType&& v)
{
	return Detail::ValueDeductor{}(std::forward<NativeType>(v));
}
//TODO:
//template<typename NativeType>
//inline Value MakeUninitialized()
//{
//	return Detail::ValueDeductor{}(std::forward<NativeType>());
//}

//
// Change value internals
//

//
// Query value and type properties
//

// Evaluate the value as either true or false.
bool EvaluateValueAsBoolean(const Value&);
// Evaluate the value as an integer or throw exception.
int EvaluateValueAsInteger(const Value&);

} // namespace Util
} // namespace CoilCl
