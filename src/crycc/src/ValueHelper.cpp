// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/ValueHelper.h>

#include <CryCC/SubValue/OffsetValue.h>
#include <CryCC/SubValue/NilValue.h>
#include <CryCC/SubValue/ReferenceValue.h>
#include <CryCC/SubValue/PointerValue.h>
#include <CryCC/SubValue/BuiltinValue.h>
#include <CryCC/SubValue/ArrayValue.h>
#include <CryCC/SubValue/RecordValue.h>

namespace Util
{

using namespace CryCC::SubValue::Valuedef;

Value MakeString(const std::string& v)
{
	std::vector<char> ve(v.begin(), v.end());
	ve.shrink_to_fit();
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::CHAR);
	return Value{ TypeFacade{ builtin }, Value::ValueVariantMulti{ std::move(ve) }, ve.size() };
}

Value MakeInt(int v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::INT);
	return Value{ TypeFacade{ builtin }, Value::ValueVariantSingle{ std::move(v) } };
}

Value MakeFloat(float v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::FLOAT);
	return Value{ TypeFacade{ builtin }, Value::ValueVariantSingle{ std::move(v) } };
}

Value MakeDouble(double v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::DOUBLE);
	return Value{ TypeFacade{ builtin }, Value::ValueVariantSingle{ std::move(v) } };
}

Value MakeChar(char v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::CHAR);
	return Value{ TypeFacade{ builtin }, Value::ValueVariantSingle{ std::move(v) } };
}

Value MakeBool(bool v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::BOOL);
	return Value{ TypeFacade{ builtin }, Value::ValueVariantSingle{ std::move(v) } };
}

Value MakeIntArray(int v[])
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::INT);
	std::vector<int> ve(v, v + sizeof(v) / sizeof(v[0]));
	return Value{ TypeFacade{ builtin }, Value::ValueVariantMulti{ std::move(ve) }, ve.size() };
}

Value MakeIntArray(std::vector<int> v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::INT);
	return Value{ TypeFacade{ builtin }, Value::ValueVariantMulti{ v }, v.size() };
}

Value MakeFloatArray(std::vector<float> v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::FLOAT);
	return Value{ TypeFacade{ builtin }, Value::ValueVariantMulti{ v }, v.size() };
}

Value MakeDoubleArray(std::vector<double> v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::DOUBLE);
	return Value{ TypeFacade{ builtin }, Value::ValueVariantMulti{ v }, v.size() };
}

Value MakeBoolArray(std::vector<bool> v)
{
	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::BOOL);
	return Value{ TypeFacade{ builtin }, Value::ValueVariantMulti{ v }, v.size() };
}

Value MakePointer(Value&& v)
{
	return Value{ TypeFacade{ MakePointerType(v.Type()) }, std::move(v) };
}

Value MakeStruct(RecordValue&& v, const std::string structName)
{
	return Value{ TypeFacade{ MakeRecordType(structName, RecordType::Specifier::STRUCT) }, std::move(v) };
}

Value MakeUnion(RecordValue&& v, const std::string structName)
{
	return Value{ TypeFacade{ MakeRecordType(structName, RecordType::Specifier::UNION) }, std::move(v) };
}

////////

Value2 MakeUninitialized()
{
	return Value2{ std::make_shared<NilType>() };
}

Value2 MakeVoid()
{
	return Value2{ MakeBuiltinType(BuiltinType::Specifier::VOID_T) };
}

Value2 MakeOffset(Value2& value, size_t offset)
{
	return Value2{ std::make_shared<NilType>(), OffsetValue{ value, offset } };
}

namespace Detail
{

template<BuiltinType::Specifier Specifier, typename Type>
Value2 MakeBuiltinImpl(const Type& value)
{
	return Value2{ MakeBuiltinType(Specifier), BuiltinValue{ value } };
}

} // namespace Detail

Value2 MakeSignedChar(signed char v)
{
	return Detail::MakeBuiltinImpl<BuiltinType::Specifier::SIGNED_CHAR_T>(v);
}

Value2 MakeUnsignedChar(unsigned char v)
{
	return Detail::MakeBuiltinImpl<BuiltinType::Specifier::UNSIGNED_CHAR_T>(v);
}

Value2 MakeShort(short v)
{
	return Detail::MakeBuiltinImpl<BuiltinType::Specifier::SHORT_T>(v);
}

Value2 MakeUnsignedShort(unsigned short v)
{
	return Detail::MakeBuiltinImpl<BuiltinType::Specifier::UNSIGNED_SHORT_T>(v);
}

Value2 MakeUnsignedInt(unsigned int v)
{
	return Detail::MakeBuiltinImpl<BuiltinType::Specifier::UNSIGNED_INT_T>(v);
}

Value2 MakeLong(long v)
{
	return Detail::MakeBuiltinImpl<BuiltinType::Specifier::LONG_T>(v);
}

Value2 MakeUnsignedLong(unsigned long v)
{
	return Detail::MakeBuiltinImpl<BuiltinType::Specifier::UNSIGNED_LONG_T>(v);
}

// Value2 MakeBool2(bool v)
// {
//  return Detail::MakeBuiltinImpl<BuiltinType::Specifier::BOOL_T>(v);
// }

Value2 MakeChar2(char v)
{
	return Value2{ MakeBuiltinType(BuiltinType::Specifier::CHAR_T), BuiltinValue{ v } };
}

Value2 MakeInt2(int v)
{
	return Value2{ MakeBuiltinType(BuiltinType::Specifier::INT_T), BuiltinValue{ v } };
}

Value2 MakeFloat2(float v)
{
	return Value2{ MakeBuiltinType(BuiltinType::Specifier::FLOAT_T), BuiltinValue{ v } };
}

Value2 MakeDouble2(double v)
{
	return Value2{ MakeBuiltinType(BuiltinType::Specifier::DOUBLE_T), BuiltinValue{ v } };
}

Value2 MakeLongDouble(long double v)
{
	return Value2{ MakeBuiltinType(BuiltinType::Specifier::LONG_DOUBLE_T), BuiltinValue{ v } };
}

namespace Detail
{

template<BuiltinType::Specifier Specifier, typename Type>
Value2 MakeArrayImpl(const Type& value)
{
	auto arrayElement = Util::MakeBuiltinType(Specifier);
	return Value2{ std::make_shared<ArrayType>(value.size(), std::move(arrayElement)), ArrayValue{ value.cbegin(), value.cend() } };
}

} // namespace Detail

Value2 MakeCharArray2(const std::vector<char>& v)
{
	return Detail::MakeArrayImpl<BuiltinType::Specifier::CHAR_T>(v);
}

Value2 MakeShortArray2(const std::vector<short>& v)
{
	return Detail::MakeArrayImpl<BuiltinType::Specifier::SHORT_T>(v);
}

Value2 MakeIntArray2(const std::vector<int>& v)
{
	return Detail::MakeArrayImpl<BuiltinType::Specifier::INT_T>(v);
}

Value2 MakeLongArray2(const std::vector<long>& v)
{
	return Detail::MakeArrayImpl<BuiltinType::Specifier::LONG_T>(v);
}

Value2 MakeUnsignedCharArray2(const std::vector<unsigned char>& v)
{
	return Detail::MakeArrayImpl<BuiltinType::Specifier::UNSIGNED_CHAR_T>(v);
}

Value2 MakeUnsignedShortArray2(const std::vector<unsigned short>& v)
{
	return Detail::MakeArrayImpl<BuiltinType::Specifier::UNSIGNED_SHORT_T>(v);
}

Value2 MakeUnsignedIntArray2(const std::vector<unsigned int>& v)
{
	return Detail::MakeArrayImpl<BuiltinType::Specifier::UNSIGNED_INT_T>(v);
}

Value2 MakeUnsignedLongArray2(const std::vector<unsigned long>& v)
{
	return Detail::MakeArrayImpl<BuiltinType::Specifier::UNSIGNED_LONG_T>(v);
}

Value2 MakeFloatArray2(const std::vector<float>& v)
{
	return Detail::MakeArrayImpl<BuiltinType::Specifier::FLOAT_T>(v);
}

Value2 MakeDoubleArray2(const std::vector<double>& v)
{
	return Detail::MakeArrayImpl<BuiltinType::Specifier::DOUBLE_T>(v);
}

Value2 MakeString2(const std::string& v)
{
	return Detail::MakeArrayImpl<BuiltinType::Specifier::CHAR_T>(v);
}

std::string ValueCastString(const Value2& value)
{
	const auto strArr = ValueCastArray<char>(value);
	return { strArr.begin(), strArr.end() };
}

size_t MultiElementSize(const Value2& value)
{
	// Passed an offset value.
	if (value.Identifier() == OffsetValue::value_category_identifier) {
		const auto& refValue = value.As<OffsetValue>()->NativeValue().Value();
		return refValue.ElementCount<ArrayValue>();
	}

	return value.ElementCount<ArrayValue>();
}

bool MultiElementEmpty(const Value2& value)
{
	// Passed an offset value.
	if (value.Identifier() == OffsetValue::value_category_identifier) {
		const auto& refValue = value.As<OffsetValue>()->NativeValue().Value();
		return refValue.ElementEmpty<ArrayValue>();
	}

	return value.ElementEmpty<ArrayValue>();
}

// Evaluate value as boolean if conversion is possible. If the conversion
// is not possible, an exception is thrown and caught here. In that case
// the evaluator returns with a negative result.
bool EvaluateValueAsBoolean(const Value& value)
{
	try { return value.As<int>(); }
	catch (const Value::InvalidTypeCastException&) {}
	return false;
}

// Convert value as integer. If the conversion fails an exception
// is thrown upwards to the caller.
int EvaluateValueAsInteger(const Value& value)
{
	return value.As<int>();
}

// Evaluate value as boolean if conversion is possible. If the conversion
// is not possible, an exception is thrown and caught here. In that case
// the evaluator returns with a negative result.
bool EvaluateValueAsBoolean(const Value2& value)
{
	try { return Util::ValueCastNative<int>(value); }
	catch (const InvalidTypeCastException&) {}
	return false;
}

// Convert value as integer. If the conversion fails an exception
// is thrown upwards to the caller.
int EvaluateValueAsInteger(const Value2& value)
{
	return Util::ValueCastNative<int>(value);
}

} // namespace Util
