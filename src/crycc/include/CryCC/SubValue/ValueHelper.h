// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/SubValue/Valuedef.h>
//#include <CryCC/SubValue/AutoValue.h> // TODO: remove

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

Value MakeUninitialized();
Value MakeVoid();
Value MakeOffset(Value&, size_t);
Value MakeSignedChar(signed char);
Value MakeUnsignedChar(unsigned char);
Value MakeShort(short);
Value MakeUnsignedShort(unsigned short);
Value MakeUnsignedInt(unsigned int);
Value MakeLong(long);
Value MakeUnsignedLong(unsigned long);
Value MakeLongDouble(double);
//Value MakeUnsignedLongDouble(double);
Value MakeString(const std::string&);

Value MakeBool(bool);
Value MakeChar(char);
Value MakeInt(int);
Value MakeFloat(float);
Value MakeDouble(double);
Value MakeBoolArray(const std::vector<bool>&);
Value MakeCharArray(const std::vector<char>&);
Value MakeShortArray(const std::vector<short>&);
Value MakeIntArray(const std::vector<int>&);
Value MakeLongArray(const std::vector<long>&);
Value MakeUnsignedCharArray(const std::vector<unsigned char>&);
Value MakeUnsignedShortArray(const std::vector<unsigned short>&);
Value MakeUnsignedIntArray(const std::vector<unsigned int>&);
Value MakeUnsignedLongArray(const std::vector<unsigned long>&);
Value MakeFloatArray(const std::vector<float>&);
Value MakeDoubleArray(const std::vector<double>&);

// FUTURE: Take name as optional
template<typename... ValueType>
Value MakeStruct(const std::string name, ValueType&&... args)
{
	RecordValue record;
	int i = 0;

	for (auto v : { args... }) {
		record.AddField(i++, std::move(v));
	}

	return Value{ MakeRecordType(name, RecordType::Specifier::STRUCT), std::move(record) };
}

// FUTURE: Take name as optional
template<typename... ValueType>
Value MakeUnion(const std::string name, ValueType&&... args)
{
	RecordValue record;
	int i = 0;

	for (auto v : { args... }) {
		record.AddField(i++, std::move(v));
	}

	return Value{ MakeRecordType(name, RecordType::Specifier::UNION), std::move(record) };
}

namespace Detail
{

template<typename NativeType, typename ValueCategory>
auto ValueCastNativeImpl(const Value& value)
{
	return value.As<ValueCategory, NativeType>();
}

} // namespace Detail

// Extract native value from value.
template<typename NativeType>
auto ValueCastNative(const Value& value)
{
	return Detail::ValueCastNativeImpl<NativeType, BuiltinValue>(value);
}

// Extract array from value.
template<typename NativeType>
auto ValueCastArray(const Value& value)
{
	return Detail::ValueCastNativeImpl<NativeType, ArrayValue>(value);
}

// Extract string from value.
std::string ValueCastString(const Value&);

// Retrieve multi element value at position.
template<typename ReturnType, auto Offset = 0>
ReturnType MultiElementAt(const Value& value)
{
	// Passed an offset value.
	if (value.Identifier() == OffsetValue::value_category_identifier) {
		const auto offset = value.As<OffsetValue>()->NativeValue().Offset();
		auto& refValue = value.As<OffsetValue>()->NativeValue().Value();
		return refValue.At<ArrayValue, ReturnType>(offset);
	}

	return value.At<ArrayValue, ReturnType, Offset>();
}

// Retrieve multi element size.
size_t MultiElementSize(const Value&);

// Test if multi element value is empty.
bool MultiElementEmpty(const Value&);

// Replace item in multi element value.
template<auto Offset = 0, typename Type>
void MultiElementEmplace(const Value& value, Type&& newval)
{
	// Passed an offset value.
	if (value.Identifier() == OffsetValue::value_category_identifier) {
		const auto offset = value.As<OffsetValue>()->NativeValue().Offset();
		auto& refValue = value.As<OffsetValue>()->NativeValue().Value();
		refValue.Emplace<ArrayValue>(offset, std::forward<Type>(newval));
		return;
	}

	value.Emplace<ArrayValue, Offset>(std::forward<Type>(newval));
}

//
// Query value and type properties.
//

// Evaluate the 'value' as either true or false.
bool EvaluateValueAsBoolean(const Value&);
// Evaluate the 'value' as an integer or throw exception.
int EvaluateValueAsInteger(const Value&);

// Evaluate the 'value' as either true or false.
bool EvaluateValueAsBoolean(const Value&);
// Evaluate the 'value' as an integer or throw exception.
int EvaluateValueAsInteger(const Value&);

//
// Type facade forwarders.
//

inline bool IsVoid(const Value& value) noexcept { return IsVoid(value.Type()); }
inline bool IsIntegral(const Value& value) noexcept { return IsIntegral(value.Type()); }
inline bool IsFloatingPoint(const Value& value) noexcept { return IsFloatingPoint(value.Type()); }
inline bool IsArray(const Value& value) noexcept { return IsArray(value.Type()); }
inline bool IsEnum(const Value& value) noexcept { return IsEnum(value.Type()); }
inline bool IsStruct(const Value& value) noexcept { return IsStruct(value.Type()); }
inline bool IsUnion(const Value& value) noexcept { return IsUnion(value.Type()); }
inline bool IsClass(const Value& value) noexcept { return IsClass(value.Type()); }
inline bool IsRecord(const Value& value) noexcept { return IsRecord(value.Type()); }
inline bool IsPointer(const Value& value) noexcept { return IsPointer(value.Type()); }
inline bool IsInline(const Value& value) noexcept { return IsInline(value.Type()); }
inline bool IsSensitive(const Value& value) noexcept { return IsSensitive(value.Type()); }
inline bool IsStatic(const Value& value) noexcept { return IsStatic(value.Type()); }
inline bool IsExtern(const Value& value) noexcept { return IsExtern(value.Type()); }
inline bool IsRegister(const Value& value) noexcept { return IsRegister(value.Type()); }
inline bool IsConst(const Value& value) noexcept { return IsConst(value.Type()); }
inline bool IsVolatile(const Value& value) noexcept { return IsVolatile(value.Type()); }
inline bool IsSigned(const Value& value) noexcept { return IsSigned(value.Type()); }
inline bool IsUnsigned(const Value& value) noexcept { return IsUnsigned(value.Type()); }

} // namespace Util
