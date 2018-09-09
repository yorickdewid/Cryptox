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

} // namespace Util
