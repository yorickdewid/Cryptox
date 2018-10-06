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
Value2 MakeLongDouble(double);
//Value2 MakeUnsignedLongDouble(double);
Value MakeString(const std::string&);
//Value2 MakeString2(const std::string&); //TODO:
//Value2 MakeStringArray(std::vector<std::string>);
//Value2 MakeStringArray2(std::vector<std::string>); //TODO:
Value MakeIntArray(int v[]);
Value MakeIntArray(std::vector<int>);
Value MakeFloatArray(std::vector<float>);
Value MakeDoubleArray(std::vector<double>);
Value MakeBoolArray(std::vector<bool>);
Value MakePointer(Value&&);
Value MakeStruct(RecordValue&&, const std::string structName = {});
Value MakeUnion(RecordValue&&, const std::string structName = {});

// Value MakeBool2(bool); //TODO
Value2 MakeChar2(char);
Value2 MakeInt2(int);
Value2 MakeFloat2(float);
Value2 MakeDouble2(double);
Value2 MakeIntArray2(const std::vector<int>&);
Value2 MakeFloatArray2(const std::vector<float>&);
Value2 MakeDoubleArray2(const std::vector<double>&);

// FUTURE: Take name as optional
template<typename... ValueType>
Value2 MakeStruct2(const std::string name, ValueType&&... args)
{
	RecordValue record;
	int i = 0;
	for (auto v : { args... }) {
		record.AddField(i++, std::move(v));
	}
	return Value2{ MakeRecordType(name, RecordType::Specifier::STRUCT), std::move(record) };
}

// FUTURE: Take name as optional
template<typename... ValueType>
Value2 MakeUnion2(const std::string name, ValueType&&... args)
{
	RecordValue record;
	int i = 0;
	for (auto v : { args... }) {
		record.AddField(i++, std::move(v));
	}
	return Value2{ MakeRecordType(name, RecordType::Specifier::UNION), std::move(record) };
}

template<typename NativeType>
inline NativeType ValueCast(const Value2& value)
{
	//TODO: Add other value categories
	if constexpr (BuiltinValue::has_type_v<std::decay_t<NativeType>>) {
		return value.As<BuiltinValue, NativeType>();
	}
	else {
		static_assert(0, "Not an castable type");
	}
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

// Evaluate the 'value' as either true or false.
bool EvaluateValueAsBoolean(const Value2&);
// Evaluate the 'value' as an integer or throw exception.
int EvaluateValueAsInteger(const Value2&);

//
// Type facade forwarders.
//

inline bool IsVoid(const Value2& value) noexcept { return IsVoid(value.Type()); }
inline bool IsIntegral(const Value2& value) noexcept { return IsIntegral(value.Type()); }
inline bool IsFloatingPoint(const Value2& value) noexcept { return IsFloatingPoint(value.Type()); }
inline bool IsArray(const Value2& value) noexcept { return IsArray(value.Type()); }
inline bool IsEnum(const Value2& value) noexcept { return IsEnum(value.Type()); }
inline bool IsStruct(const Value2& value) noexcept { return IsStruct(value.Type()); }
inline bool IsUnion(const Value2& value) noexcept { return IsUnion(value.Type()); }
inline bool IsClass(const Value2& value) noexcept { return IsClass(value.Type()); }
inline bool IsRecord(const Value2& value) noexcept { return IsRecord(value.Type()); }
inline bool IsPointer(const Value2& value) noexcept { return IsPointer(value.Type()); }
inline bool IsInline(const Value2& value) noexcept { return IsInline(value.Type()); }
inline bool IsSensitive(const Value2& value) noexcept { return IsSensitive(value.Type()); }
inline bool IsStatic(const Value2& value) noexcept { return IsStatic(value.Type()); }
inline bool IsExtern(const Value2& value) noexcept { return IsExtern(value.Type()); }
inline bool IsRegister(const Value2& value) noexcept { return IsRegister(value.Type()); }
inline bool IsConst(const Value2& value) noexcept { return IsConst(value.Type()); }
inline bool IsVolatile(const Value2& value) noexcept { return IsVolatile(value.Type()); }
inline bool IsSigned(const Value2& value) noexcept { return IsSigned(value.Type()); }
inline bool IsUnsigned(const Value2& value) noexcept { return IsUnsigned(value.Type()); }

} // namespace Util
