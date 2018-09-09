// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/SubValue/Valuedef.h>
#include <CryCC/SubValue/AutoValue.h>

//TODO: replace with function.
#ifdef CRY_DEBUG
#include <iostream>
#define DUMP_VALUE(v) \
	std::cout << v.Type()->TypeName() << " >> " << v.Print() << std::endl;
#endif

namespace CryCC::SubValue::Valuedef
{
	
// Request the value at the record field with 'name'.
std::shared_ptr<Value> RecordMemberValue(Value& value, const std::string& name);

} // namespace CryCC::SubValue::Valuedef

// The value helper functions ease the creation and modification of the
// value objects as well as some functions to query specific properties.
// In any case should the helper functions be used instead of accessing
// the value objects directly. The helper functions are designed to support
// the most common value types as such that the caller does not need to
// cast the value into a specific type.

namespace Util
{

using namespace CryCC::SubValue::Valuedef;
using namespace CryCC::SubValue::Typedef;

Value2 MakeUninitialized();
Value2 MakeVoid();
Value MakeBool(bool);
Value MakeChar(char);
Value2 MakeSignedChar(signed char);
Value2 MakeUnsignedChar(unsigned char);
Value2 MakeShort(short);
Value2 MakeUnsignedShort(unsigned short);
Value MakeInt(int);
Value2 MakeUnsignedInt(unsigned int);
Value2 MakeLong(long);
Value2 MakeUnsignedLong(unsigned long);
Value MakeFloat(float);
Value MakeDouble(double);
// Value MakeLongDouble(double);
// Value MakeUnsignedLongDouble(double);
Value MakeString(const std::string&);
//Value MakeStringArray(std::vector<std::string>);
Value MakeIntArray(int v[]);
Value MakeIntArray(std::vector<int>);
Value MakeFloatArray(std::vector<float>);
Value MakeDoubleArray(std::vector<double>);
Value MakeBoolArray(std::vector<bool>);
Value MakePointer(Value&&);
Value MakeStruct(RecordValue&&, const std::string structName = {});
Value MakeUnion(RecordValue&&, const std::string structName = {});

// Value MakeBool2(bool);
Value2 MakeChar2(char);
Value2 MakeInt2(int);
Value2 MakeFloat2(float);
Value2 MakeDouble2(double);
// Value2 MakeIntArray2(int v[]);
// Value2 MakeIntArray2(std::vector<int>);
// Value2 MakeFloatArray2(std::vector<float>);
// Value2 MakeDoubleArray2(std::vector<double>);

template<typename NativeType>
inline NativeType ValueCast(const Value2& value)
{
	return value.As<BuiltinValue, NativeType>();
}

//
// Create implicit value with automatic type.
//

template<typename NativeType>
inline Value MakeAutoValue(NativeType&& v)
{
	return std::invoke(Detail::ValueDeductor{}, std::forward<NativeType>(v));
}

//
// Query value and type properties.
//

// Evaluate the 'value' as either true or false.
bool EvaluateValueAsBoolean(const Value&);
// Evaluate the 'value' as an integer or throw exception.
int EvaluateValueAsInteger(const Value&);

} // namespace Util
