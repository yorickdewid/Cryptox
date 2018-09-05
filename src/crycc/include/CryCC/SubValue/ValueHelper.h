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

//TODO: remove
#define CaptureValue(s) Util::CaptureValueRaw(std::move(s))

namespace CryCC
{
namespace SubValue
{
namespace Valuedef
{
	
// Request the value at the record field with 'name'.
std::shared_ptr<Value> RecordMemberValue(Value& value, const std::string& name);

} // namespace Valuedef
} // namespace SubValue
} // namespace CryCC

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

// Value MakeUninitialized();
// Value MakeVoid();
Value MakeBool(bool);
Value MakeChar(char);
// Value MakeSignedChar(signed char);
// Value MakeUnsignedChar(unsigned char);
// Value MakeShort(short);
// Value MakeUnsignedShort(unsigned short);
Value MakeInt(int);
// Value MakeUnsignedInt(unsigned int);
// Value MakeLong(long);
// Value MakeUnsignedLong(unsigned long);
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

//
// Create implicit value with automatic type.
//

template<typename NativeType>
inline Value CaptureValueRaw(NativeType&& v) //TODO: remove
{
	return Detail::ValueDeductor{}(std::forward<NativeType>(v));
}
template<typename NativeType>
inline Value MakeAutoValue(NativeType&& v)
{
	Detail::ValueDeductor v;
	return v(std::forward<NativeType>(v));
}

//
// Query value and type properties.
//

// Evaluate the 'value' as either true or false.
bool EvaluateValueAsBoolean(const Value&);
// Evaluate the 'value' as an integer or throw exception.
int EvaluateValueAsInteger(const Value&);

} // namespace Util
