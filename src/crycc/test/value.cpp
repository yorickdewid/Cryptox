// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue.h>

#include <boost/test/unit_test.hpp>

//
// Key         : Value
// Test        : Value definition unitttest
// Type        : unit
// Description : Unit test of the value type system. Since the value definition 
//               system is the backbone of the compiler it demands for almost
//               full test coverage.
//

//TODO:
// - Make Bool
// - Autovalue

using namespace CryCC::SubValue::Typedef;
using namespace CryCC::SubValue::Valuedef;

BOOST_AUTO_TEST_SUITE(ValueDefinition)

BOOST_AUTO_TEST_CASE(ValueRework2Dissected)
{
	// Basic value construction.
	{
		auto builtin = Util::MakeBuiltinType(BuiltinType::Specifier::INT_T);
		TypeFacade facade{ builtin };
		Value2 val{ std::move(facade), BuiltinValue{ 8612 } };

		BOOST_CHECK(val.Initialized());
		BOOST_REQUIRE_EQUAL("8612", val.ToString());
		BOOST_REQUIRE_EQUAL(8612, (val.As<BuiltinValue, int>()));
	}

	// Basic inline construction.
	{
		Value2 val{ Util::MakeBuiltinType(BuiltinType::Specifier::FLOAT_T), BuiltinValue{ 434.86231f } };

		BOOST_CHECK(val.Initialized());
		BOOST_REQUIRE_EQUAL(434.86231f, (val.As<BuiltinValue, float>()));
	}

	// Cast and invalid cast.
	{
		auto builtin = Util::MakeBuiltinType(BuiltinType::Specifier::INT_T);
		Value2 val{ TypeFacade{ builtin }, BuiltinValue{ 919261 } };

		BOOST_CHECK(val.Initialized());
		BOOST_CHECK(val.Type().HasValue());
		BOOST_REQUIRE_NO_THROW((val.As<BuiltinValue, int>()));
		BOOST_CHECK_THROW((val.As<BuiltinValue, char>()), InvalidTypeCastException);
	}

	// Copy and move.
	{
		auto builtin = Util::MakeBuiltinType(BuiltinType::Specifier::SHORT_T);
		Value2 val{ TypeFacade{ builtin } };
		Value2 val2{ val };
		Value2 val3{ std::move(val2) };

		BOOST_CHECK(!val);
		BOOST_CHECK(!val3);
		BOOST_CHECK(!val.Initialized());
		BOOST_CHECK(!val3.Initialized());
		BOOST_CHECK_THROW((val.As<BuiltinValue, char>()), InvalidTypeCastException);
		BOOST_CHECK_THROW((val3.As<BuiltinValue, char>()), InvalidTypeCastException);
	}

	// Basic array 1.
	{
		ArrayValue a{ 8612, 812, 2383, 96, 12 };
		std::vector<int> _a = a.As<int>();

		auto arrayElement = Util::MakeBuiltinType(BuiltinType::Specifier::INT_T);
		auto arrayType = std::make_shared<ArrayType>(5, std::move(arrayElement));

		Value2 val{ std::move(arrayType), std::move(a) };

		BOOST_CHECK(val.Initialized());
		BOOST_CHECK(!val.ElementEmpty<ArrayValue>());
		//BOOST_CHECK_EQUAL(val.ElementCount<ArrayValue>(), val.Type().ArraySize());

		std::vector<int> b = val.As<ArrayValue, int>();

		BOOST_REQUIRE_EQUAL_COLLECTIONS(_a.begin(), _a.end(), b.begin(), b.end());
	}

	// Basic array 2.
	{
		ArrayValue a{ 'X', 'O', 'A', 'N', 'B' };
		std::vector<char> _a = a.As<char>();

		auto arrayElement = Util::MakeBuiltinType(BuiltinType::Specifier::CHAR_T);
		auto arrayType = std::make_shared<ArrayType>(5, std::move(arrayElement));

		Value2 val{ std::move(arrayType), std::move(a) };

		BOOST_CHECK(val.Initialized());
		BOOST_CHECK(!val.ElementEmpty<ArrayValue>());
		//BOOST_CHECK_EQUAL(val.ElementCount<ArrayValue>(), val.Type().ArraySize());

		std::vector<char> b = val.As<ArrayValue, char>();

		BOOST_REQUIRE_EQUAL_COLLECTIONS(_a.begin(), _a.end(), b.begin(), b.end());
	}

	// Compare values.
	{
		Value2 val1{ Util::MakeBuiltinType(BuiltinType::Specifier::CHAR_T), BuiltinValue{ 12 } };

		BOOST_CHECK(val1);

		Value2 val2{ Util::MakeBuiltinType(BuiltinType::Specifier::CHAR_T), BuiltinValue{ 19 } };

		BOOST_CHECK(val2);

		Value2 val3{ Util::MakeBuiltinType(BuiltinType::Specifier::CHAR_T), BuiltinValue{ 19 } };

		BOOST_CHECK(val3);

		BOOST_REQUIRE_NE(val1, val2);
		BOOST_REQUIRE_EQUAL(val2, val3);
	}

	// Array operations.
	{
		ArrayValue a{ 12L,34L,56L,91L };
		std::vector<long> _a = a.As<long>();

		auto arrayElement = Util::MakeBuiltinType(BuiltinType::Specifier::LONG_T);
		Value2 val{ std::make_shared<ArrayType>(4, std::move(arrayElement)), std::move(a) };

		BOOST_REQUIRE_EQUAL(56L, (val.At<ArrayValue, long, 2>()));
		BOOST_REQUIRE_EQUAL(56L, (val.At<ArrayValue, long>(2)));

		val.Emplace<ArrayValue, 2>(183L);
		val.Emplace<ArrayValue>(3, 8974L);

		BOOST_REQUIRE_EQUAL(183L, (val.At<ArrayValue, long, 2>()));
		BOOST_REQUIRE_EQUAL(8974L, (val.At<ArrayValue, long>(3)));
	}
}

BOOST_AUTO_TEST_CASE(ValueRework2Declaration)
{
	//auto valStr = Util::MakeString("teststring");
	auto valInt = Util::MakeInt2(12);
	auto valFloat = Util::MakeFloat2(92.123f);
	auto valDouble = Util::MakeDouble2(87341.78263);
	auto valChar = Util::MakeChar2('K');
	//auto valBool = Util::MakeBool(true);

	//BOOST_CHECK(!valStr.Initialized());
	BOOST_CHECK(valInt.Initialized());
	BOOST_CHECK(valFloat.Initialized());
	BOOST_CHECK(valDouble.Initialized());
	BOOST_CHECK(valChar.Initialized());
	//BOOST_CHECK(!valBool.Initialized());

	//BOOST_REQUIRE_EQUAL("teststring", valStr.As<ArrayType, char>());
	BOOST_REQUIRE_EQUAL(12, Util::ValueCast<int>(valInt));
	BOOST_REQUIRE_EQUAL(92.123f, Util::ValueCast<float>(valFloat));
	BOOST_REQUIRE_EQUAL(87341.78263, Util::ValueCast<double>(valDouble));
	BOOST_REQUIRE_EQUAL('K', Util::ValueCast<char>(valChar));
	//BOOST_REQUIRE_EQUAL(true, valBool.As<BuiltinValue, bool>());
}

BOOST_AUTO_TEST_CASE(ValueRework2DeclarationArray)
{
	std::vector<int> _valIntArray{ 9, 12, 7612, 8, 112, 8 };
	std::vector<float> _valFloatArray{ 125.233f, 1.9812f, 89.8612f };
	std::vector<double> _valDoubleArray{ 1.8712, 873.655, 891.87316, 8712.8213 };
	std::vector<bool> _valBoolArray{ true, true, false, true, false, false };

	auto valIntArray = Util::MakeIntArray2(_valIntArray);
	auto valFloatArray = Util::MakeFloatArray2(_valFloatArray);
	auto valDoubleArray = Util::MakeDoubleArray2(_valDoubleArray);
	//auto valBoolArray = Util::MakeBoolArray({ true, true, false, true, false, false });

	BOOST_CHECK_EQUAL(_valIntArray.size(), valIntArray.ElementCount<ArrayValue>());
	BOOST_CHECK_EQUAL(_valFloatArray.size(), valFloatArray.ElementCount<ArrayValue>());
	BOOST_CHECK_EQUAL(_valDoubleArray.size(), valDoubleArray.ElementCount<ArrayValue>());
	//BOOST_CHECK_EQUAL(_valBoolArray.size(), valBoolArray.Type().ElementCount<ArrayValue>());

	BOOST_REQUIRE(_valIntArray == (valIntArray.As<ArrayValue, int>()));
	BOOST_REQUIRE(_valFloatArray == (valFloatArray.As<ArrayValue, float>()));
	BOOST_REQUIRE(_valDoubleArray == (valDoubleArray.As<ArrayValue, double>()));
	//BOOST_REQUIRE(_valBoolArray == valBoolArray.As<bool>());
}

BOOST_AUTO_TEST_CASE(ValueRework2AutoValue)
{
	//int _valInt = 8712;
	//float _valFloat = 7812.8612f;
	//double _valDouble = 91.72634813;
	//char _valChar = 'J';
	//bool _valBool = false;

	//volatile float _valFloat2 = 7812.8612f;
	//const char _valChar2 = 'J';

	//auto valInt = CaptureValue(_valInt);
	//auto valFloat = CaptureValue(_valFloat);
	//auto valDouble = CaptureValue(_valDouble);
	//auto valChar = CaptureValue(_valChar);
	//auto valBool = CaptureValue(_valBool);

	//auto valFloat2 = CaptureValue(_valFloat2);
	//auto valChar2 = CaptureValue(_valChar2);

	//BOOST_REQUIRE_EQUAL(_valInt, valInt.As<int>());
	//BOOST_REQUIRE_EQUAL(_valFloat, valFloat.As<float>());
	//BOOST_REQUIRE_EQUAL(_valDouble, valDouble.As<double>());
	//BOOST_REQUIRE_EQUAL(_valChar, valChar.As<char>());
	//BOOST_REQUIRE_EQUAL(_valBool, valBool.As<bool>());

	//BOOST_REQUIRE(Util::IsVolatile(valFloat2.Type()));
	//BOOST_REQUIRE(Util::IsConst(valChar2.Type()));
	//BOOST_REQUIRE_EQUAL(_valFloat2, valFloat2.As<float>());
	//BOOST_REQUIRE_EQUAL(_valChar2, valChar2.As<char>());
}

BOOST_AUTO_TEST_CASE(ValueRework2AutoMultiValue)
{
	//std::vector<int> _valIntArray{ 12, 89, 761, 86712, 7, 71, 99 };
	//std::vector<float> _valFloatArray{ 12.341f, 99.1672f, 1184.812f, 1.7263f };
	//std::vector<double> _valDoubleArray{ 923, 1192.23, 7.873123, 9.716289 };
	//std::vector<bool> _valBoolArray{ false, false, false, false, true, false, true };

	//volatile std::vector<float> _valFloatArray2 = { 9.812f, 87234.21f, 12.1872f, 9.2873f, 7.71628f };
	//const std::vector<bool> _valBoolArray2 = { true, true, false, false, true, false, true };

	//auto valIntArray = CaptureValue(_valIntArray);
	//auto valFloatArray = CaptureValue(_valFloatArray);
	//auto valDoubleArray = CaptureValue(_valDoubleArray);
	//auto valBoolArray = CaptureValue(_valBoolArray);

	//auto valFloatArray2 = CaptureValue(_valFloatArray2);
	//auto valBoolArray2 = CaptureValue(_valBoolArray2);

	//BOOST_REQUIRE(_valIntArray == valIntArray.As<std::vector<int>>());
	//BOOST_REQUIRE(_valFloatArray == valFloatArray.As<std::vector<float>>());
	//BOOST_REQUIRE(_valDoubleArray == valDoubleArray.As<std::vector<double>>());
	//BOOST_REQUIRE(_valBoolArray == valBoolArray.As<std::vector<bool>>());

	//BOOST_REQUIRE(Util::IsVolatile(valFloatArray2.Type()));
	//BOOST_REQUIRE(Util::IsConst(valBoolArray2.Type()));
	//BOOST_REQUIRE(const_cast<std::vector<float>&>(_valFloatArray2) == valFloatArray2.As<std::vector<float>>());
	//BOOST_REQUIRE(_valBoolArray2 == valBoolArray2.As<std::vector<bool>>());
}

BOOST_AUTO_TEST_CASE(ValueRework2Pointer)
{
	//{
	//	auto valInt = Util::MakeInt(1547483642);
	//	auto valPtr = Util::MakePointer(std::move(valInt));

	//	BOOST_CHECK(!valPtr.Empty());
	//	BOOST_REQUIRE(valPtr.IsReference());
	//	BOOST_REQUIRE_EQUAL(1547483642, valPtr.As<Value>().As<int>());
	//}

	//{
	//	std::vector<double> _valDoubleArray{ 923, 1192.23, 7.873123, 9.716289 };
	//	auto valDoubleArray = CaptureValue(_valDoubleArray);
	//	auto valPtr = CaptureValue(valDoubleArray);

	//	BOOST_CHECK(!valPtr.Empty());
	//	BOOST_REQUIRE(valPtr.IsReference());
	//	BOOST_REQUIRE(!valPtr.IsArray());
	//	BOOST_REQUIRE(Util::IsArray(valPtr.As<Value>().Type()));
	//	BOOST_REQUIRE(_valDoubleArray == valPtr.As<Value>().As<std::vector<double>>());
	//}
}

BOOST_AUTO_TEST_CASE(ValueRework2Record)
{
	//auto valInt = Util::MakeInt(4234761);
	//auto valFloatArray = Util::MakeFloatArray({ 125.233f, 1.9812f, 89.8612f });

	//RecordValue record;
	//record.AddField({ "i", RecordValue::AutoValue(valInt) });
	//record.AddField({ "j", RecordValue::AutoValue(valFloatArray) });

	//RecordValue record2{ record };

	//auto valStruct = Util::MakeStruct(std::move(record));
	//BOOST_REQUIRE(Util::IsStruct(valStruct.Type()));

	//BOOST_CHECK(!valStruct.Empty());
	//BOOST_CHECK(valInt == (*valStruct.As<RecordValue>().GetField("i")));
	//BOOST_REQUIRE(record2 == valStruct.As<RecordValue>());
}

BOOST_AUTO_TEST_CASE(ValueRework2Replace)
{
	//{
	//	auto valInt = Util::MakeInt(982734);
	//	valInt = Util::MakeInt(17);
	//	BOOST_REQUIRE_EQUAL(17, valInt.As<int>());
	//	BOOST_REQUIRE(BuiltinType::Specifier::INT == valInt.Type().DataType<BuiltinType>()->TypeSpecifier());
	//	valInt = Util::MakeInt(7862138);
	//	BOOST_REQUIRE(BuiltinType::Specifier::INT == valInt.Type().DataType<BuiltinType>()->TypeSpecifier());
	//	BOOST_REQUIRE_THROW(valInt = Util::MakeFloat(12.23f), Value::InvalidTypeCastException);
	//}

	//{
	//	auto valDouble = CaptureValue(8273.87123);
	//	Value val2 = valDouble;
	//	BOOST_CHECK(valDouble);
	//	BOOST_REQUIRE_EQUAL(8273.87123, val2.As<double>());
	//}
}

BOOST_AUTO_TEST_CASE(ValueRework2Misc)
{
	auto valNil = Util::MakeUninitialized();

	BOOST_REQUIRE(!valNil.Initialized());

	//{
	//	BOOST_REQUIRE(Util::IsIntegral(Util::MakeInt(722).Type()));
	//	BOOST_REQUIRE(!Util::IsIntegral(Util::MakeFloat(8.851283f).Type()));
	//	BOOST_REQUIRE(Util::IsFloatingPoint(Util::MakeFloat(8.851283f).Type()));
	//}

	//{
	//	BOOST_REQUIRE(Util::EvaluateValueAsBoolean(Util::MakeInt(722)));
	//	BOOST_REQUIRE(!Util::EvaluateValueAsBoolean(Util::MakeInt(0)));
	//	BOOST_REQUIRE_EQUAL(762386, Util::EvaluateValueAsInteger(Util::MakeInt(762386)));
	//	BOOST_REQUIRE_EQUAL(0, Util::EvaluateValueAsInteger(Util::MakeInt(0)));
	//	BOOST_REQUIRE_THROW(Util::EvaluateValueAsInteger(Util::MakeIntArray({ 12,23 })), Value::UninitializedValueException);
	//	BOOST_REQUIRE_THROW(Util::EvaluateValueAsInteger(Util::MakeFloat(8.12f)), Value::InvalidTypeCastException);
	//}
}

BOOST_AUTO_TEST_CASE(ValDefRecordMemberValue)
{
	//// Test if existing value can be accessed via 'RecordMemberValue'.
	//{
	//	auto valInt = Util::MakeInt(7261);
	//	auto valString = Util::MakeString("teststring");

	//	RecordValue record;
	//	record.AddField({ "int", RecordValue::AutoValue(valInt) });
	//	record.AddField({ "str", RecordValue::AutoValue(valString) });

	//	auto valUnion = Util::MakeUnion(std::move(record));

	//	auto valMemInt = RecordMemberValue(valUnion, "int");
	//	BOOST_REQUIRE(valInt == (*valMemInt));
	//}

	//// Create record value and member in value.
	//{
	//	auto record = Util::MakeRecordType("union", RecordType::Specifier::UNION);
	//	record->AddField("int", std::make_shared<BaseType2::element_type>(Util::MakeBuiltinType(BuiltinType::Specifier::INT)));
	//	record->AddField("char", std::make_shared<BaseType2::element_type>(Util::MakeBuiltinType(BuiltinType::Specifier::CHAR)));

	//	Value valUninit{ TypeFacade{ record } };

	//	auto valMemInt = RecordMemberValue(valUninit, "int");
	//	BOOST_CHECK(valMemInt->Empty());
	//	(*valMemInt) = Util::MakeInt(17);

	//	auto valMemInt2 = RecordMemberValue(valUninit, "int");
	//	BOOST_REQUIRE_EQUAL(Util::MakeInt(17), (*valMemInt2));
	//}
}

BOOST_AUTO_TEST_CASE(ValDefReworkSerialize)
{
	//using namespace Util;

	//{
	//	const Value val = CaptureValue(7962193);
	//	Cry::ByteArray buffer = val.Serialize();

	//	const Value val2 = ValueFactory::MakeValue(buffer);
	//	BOOST_REQUIRE_EQUAL(val, val2);
	//}

	//{
	//	const Value val = CaptureValue('O');
	//	Cry::ByteArray buffer = val.Serialize();

	//	const Value val2 = ValueFactory::MakeValue(buffer);
	//	BOOST_REQUIRE_EQUAL(val, val2);
	//}

	//{
	//	const Value val = MakeIntArray({ 722, 81, 86131, 71 });
	//	Cry::ByteArray buffer = val.Serialize();

	//	const Value val2 = ValueFactory::MakeValue(buffer);
	//	BOOST_REQUIRE_EQUAL(val, val2);
	//}

	//{
	//	const Value val = Util::MakeString("teststring");
	//	Cry::ByteArray buffer = val.Serialize();

	//	const Value val2 = ValueFactory::MakeValue(buffer);
	//	BOOST_REQUIRE_EQUAL("teststring", val2.As<std::string>());
	//}

	//{
	//	RecordValue record;
	//	record.AddField({ "o", RecordValue::AutoValue(Util::MakeInt(82371)) });
	//	record.AddField({ "p", RecordValue::AutoValue(Util::MakeInt(19)) });

	//	RecordValue record2{ record };

	//	auto val = Util::MakeStruct(std::move(record));
	//	Cry::ByteArray buffer = val.Serialize();

	//	const Value val2 = ValueFactory::MakeValue(buffer);
	//	BOOST_REQUIRE_EQUAL(val, val2);
	//}

	//{
	//	auto valInt = Util::MakeInt(796162);
	//	auto val = Util::MakePointer(std::move(valInt));
	//	Cry::ByteArray buffer = val.Serialize();

	//	const Value val2 = ValueFactory::MakeValue(buffer);
	//	BOOST_REQUIRE(val2.IsReference());
	//	BOOST_REQUIRE_EQUAL(796162, val2.As<Value>().As<int>());
	//}
}

BOOST_AUTO_TEST_SUITE_END()
