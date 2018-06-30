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
	return Value{ TypeFacade{ builtin }, Value::ValueVariantMulti{ std::move(ve) } };
}
inline auto MakeInt(int v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::INT);
	return Value{ TypeFacade{ builtin }, Value::ValueVariantSingle{ std::move(v) } };
}
inline auto MakeFloat(float v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::FLOAT);
	return Value{ TypeFacade{ builtin }, Value::ValueVariantSingle{ std::move(v) } };
}
inline auto MakeDouble(double v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::DOUBLE);
	return Value{ TypeFacade{ builtin }, Value::ValueVariantSingle{ std::move(v) } };
}
inline auto MakeChar(char v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::CHAR);
	return Value{ TypeFacade{ builtin }, Value::ValueVariantSingle{ std::move(v) } };
}
inline auto MakeBool(bool v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::BOOL);
	return Value{ TypeFacade{ builtin }, Value::ValueVariantSingle{ std::move(v) } };
}

//
// Create explicit array value with automatic type
//

//inline auto MakeStringArray(std::vector<std::string> v)
//{
//	std::vector<std::vector<char>> ve(v.begin(), v.end());
//	ve.shrink_to_fit();
//	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::CHAR);
//	return Value{ TypeFacade{ builtin }, Value::ValueVariantMulti{ std::move(ve) } };
//}
inline auto MakeIntArray(int v[])
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::INT);
	std::vector<int> ve(v, v + sizeof(v) / sizeof(v[0]));
	return Value{ TypeFacade{ builtin }, Value::ValueVariantMulti{ std::move(ve) } };
}
inline auto MakeIntArray(std::vector<int> v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::INT);
	return Value{ TypeFacade{ builtin }, Value::ValueVariantMulti{ v } };
}
inline auto MakeFloatArray(std::vector<float> v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::FLOAT);
	return Value{ TypeFacade{ builtin }, Value::ValueVariantMulti{ v } };
}
inline auto MakeDoubleArray(std::vector<double> v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::DOUBLE);
	return Value{ TypeFacade{ builtin }, Value::ValueVariantMulti{ v } };
}
inline auto MakeBoolArray(std::vector<bool> v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::BOOL);
	return Value{ TypeFacade{ builtin }, Value::ValueVariantMulti{ v } };
}

//
// Create pointer to another value
//

inline auto MakePointer(Value&& v)
{
	return Value{ TypeFacade{ MakePointerType() }, std::move(v) };
}

//
// Create explicit record value with automatic type
//

inline auto MakeStruct(RecordValue&& v, const std::string structName = {})
{
	const std::string name = v.HasRecordName() ? v.RecordName() : structName;
	return Value{ TypeFacade{ MakeRecordType(name, RecordType::Specifier::STRUCT) }, std::move(v) };
}
inline auto MakeUnion(RecordValue&& v, const std::string structName = {})
{
	const std::string name = v.HasRecordName() ? v.RecordName() : structName;
	return Value{ TypeFacade{ MakeRecordType(name, RecordType::Specifier::UNION) }, std::move(v) };
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
// Query value and type properties
//

// Evaluate the value as either true or false.
bool EvaluateValueAsBoolean(const Value&);
// Evaluate the value as an integer or throw exception.
int EvaluateValueAsInteger(const Value&);

} // namespace Util
} // namespace CoilCl
