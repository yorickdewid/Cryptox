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

Value2 MakeSignedChar(signed char v)
{
	return Value2{ MakeBuiltinType(BuiltinType::Specifier::SIGNED_CHAR_T), BuiltinValue{ v } };
}

Value2 MakeUnsignedChar(unsigned char v)
{
	return Value2{ MakeBuiltinType(BuiltinType::Specifier::UNSIGNED_CHAR_T), BuiltinValue{ v } };
}

Value2 MakeShort(short v)
{
	return Value2{ MakeBuiltinType(BuiltinType::Specifier::SHORT_T), BuiltinValue{ v } };
}

Value2 MakeUnsignedShort(unsigned short v)
{
	return Value2{ MakeBuiltinType(BuiltinType::Specifier::UNSIGNED_SHORT_T), BuiltinValue{ v } };
}

Value2 MakeUnsignedInt(unsigned int v)
{
	return Value2{ MakeBuiltinType(BuiltinType::Specifier::UNSIGNED_INT_T), BuiltinValue{ v } };
}

Value2 MakeLong(long v)
{
	return Value2{ MakeBuiltinType(BuiltinType::Specifier::LONG_T), BuiltinValue{ v } };
}

Value2 MakeUnsignedLong(unsigned long v)
{
	return Value2{ MakeBuiltinType(BuiltinType::Specifier::UNSIGNED_LONG_T), BuiltinValue{ v } };
}

// Value2 MakeBool2(bool v)
// {
// 	return Value2{ MakeBuiltinType(BuiltinType::Specifier::BOOL_T), BuiltinValue{ v } };
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

// Value2 MakeIntArray2(int v[])
// {
// 	const auto builtin = MakeBuiltinType(BuiltinType::Specifier::INT);
// 	std::vector<int> ve(v, v + sizeof(v) / sizeof(v[0]));
// 	return Value{ TypeFacade{ builtin }, Value::ValueVariantMulti{ std::move(ve) }, ve.size() };
// }

Value2 MakeIntArray2(const std::vector<int>& v)
{
	auto arrayElement = Util::MakeBuiltinType(BuiltinType::Specifier::INT_T);
	return Value2{ std::make_shared<ArrayType>(v.size(), std::move(arrayElement)), ArrayValue{ v.cbegin(), v.cend() } };
}

Value2 MakeFloatArray2(const std::vector<float>& v)
{
	auto arrayElement = Util::MakeBuiltinType(BuiltinType::Specifier::FLOAT_T);
	return Value2{ std::make_shared<ArrayType>(v.size(), std::move(arrayElement)), ArrayValue{ v.cbegin(), v.cend() } };
}

Value2 MakeDoubleArray2(const std::vector<double>& v)
{
	auto arrayElement = Util::MakeBuiltinType(BuiltinType::Specifier::DOUBLE_T);
	return Value2{ std::make_shared<ArrayType>(v.size(), std::move(arrayElement)), ArrayValue{ v.cbegin(), v.cend() } };
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
	try { return Util::ValueCast<int>(value); }
	catch (const InvalidTypeCastException&) {}
	return false;
}

// Convert value as integer. If the conversion fails an exception
// is thrown upwards to the caller.
int EvaluateValueAsInteger(const Value2& value)
{
	return Util::ValueCast<int>(value);
}

} // namespace Util
