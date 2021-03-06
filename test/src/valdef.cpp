// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue.h>

#include <boost/test/unit_test.hpp>

#ifdef _OBSOLETE_

//
// Key         : Value
// Test        : Value definition unitttest
// Type        : unit
// Description : Unit test of the value type system. Since the value definition 
//               system is the backbone of the compiler it demands for almost
//               full test coverage.
//

using namespace CryCC::SubValue::Typedef;
using namespace CryCC::SubValue::Valuedef;

BOOST_AUTO_TEST_SUITE(ValueDefinition)

BOOST_AUTO_TEST_CASE(ValDefBasicReworkDissected)
{
	{
		auto builtin = Util::MakeBuiltinType(BuiltinType::Specifier::INT);
		TypeFacade facade{ builtin };
		Value val{ facade, Value::ValueVariantSingle{ 8612 } };

		BOOST_CHECK(!val.Empty());
		BOOST_REQUIRE_EQUAL(8612, val.As<int>());
	}

	{
		auto builtin = Util::MakeBuiltinType(BuiltinType::Specifier::INT);
		Value val{ TypeFacade{ builtin }, Value::ValueVariantSingle{ 919261 } };

		BOOST_CHECK(!val.Empty());
		BOOST_REQUIRE_NO_THROW(val.As<int>());
		BOOST_CHECK_THROW(val.As<char>(), Value::InvalidTypeCastException);
	}

	{
		auto builtin = Util::MakeBuiltinType(BuiltinType::Specifier::SHORT);
		Value val{ TypeFacade{ builtin } };

		BOOST_CHECK(!val);
		BOOST_CHECK(val.Empty());
		BOOST_CHECK_THROW(val.As<char>(), Value::UninitializedValueException);
	}

	{
		std::vector<int> a{ 8612, 812, 2383, 96, 12 };

		Value val{ TypeFacade{ Util::MakeBuiltinType(BuiltinType::Specifier::INT) }
			, Value::ValueVariantMulti{ a }, a.size() };

		BOOST_CHECK(!val.Empty());
		BOOST_CHECK_EQUAL(a.size(), val.Type().ArraySize());
		BOOST_CHECK(Util::IsArray(val.Type()));

		std::vector<int> b = val.As<std::vector<int>>();

		BOOST_REQUIRE(a == b);
	}

	{
		std::vector<char> a{ 'X', 'O', 'A', 'N', 'B' };

		Value val{ TypeFacade{ Util::MakeBuiltinType(BuiltinType::Specifier::CHAR) }
			, Value::ValueVariantMulti{ a }, a.size() };

		BOOST_CHECK(!val.Empty());
		BOOST_CHECK_EQUAL(a.size(), val.Type().ArraySize());
		BOOST_CHECK(Util::IsArray(val.Type()));

		std::vector<char> b = val.As<std::vector<char>>();

		BOOST_REQUIRE(a == b);
	}

	{
		Value val1{ TypeFacade{ Util::MakeBuiltinType(BuiltinType::Specifier::CHAR) }
			, Value::ValueVariantSingle{ 19 } };

		BOOST_CHECK(val1);

		Value val2{ TypeFacade{ Util::MakeBuiltinType(BuiltinType::Specifier::CHAR) }
			, Value::ValueVariantSingle{ 12 } };

		BOOST_CHECK(val2);

		Value val3{ TypeFacade{ Util::MakeBuiltinType(BuiltinType::Specifier::CHAR) }
			, Value::ValueVariantSingle{ 19 } };

		BOOST_CHECK(val3);

		BOOST_REQUIRE_NE(val1, val2);
		BOOST_REQUIRE_EQUAL(val1, val3);
	}
}

BOOST_AUTO_TEST_CASE(ValDefReworkDeclaration)
{
	auto valStr = Util::MakeString("teststring");
	auto valInt = Util::MakeInt(12);
	auto valFloat = Util::MakeFloat(92.123f);
	auto valDouble = Util::MakeDouble(87341);
	auto valChar = Util::MakeChar('K');
	auto valBool = Util::MakeBool(true);

	BOOST_CHECK(!valStr.Empty());
	BOOST_CHECK(!valInt.Empty());
	BOOST_CHECK(!valFloat.Empty());
	BOOST_CHECK(!valDouble.Empty());
	BOOST_CHECK(!valChar.Empty());
	BOOST_CHECK(!valBool.Empty());

	BOOST_REQUIRE_EQUAL("teststring", valStr.As<std::string>());
	BOOST_REQUIRE_EQUAL(12, valInt.As<int>());
	BOOST_REQUIRE_EQUAL(92.123f, valFloat.As<float>());
	BOOST_REQUIRE_EQUAL(87341, valDouble.As<double>());
	BOOST_REQUIRE_EQUAL('K', valChar.As<char>());
	BOOST_REQUIRE_EQUAL(true, valBool.As<bool>());
}

BOOST_AUTO_TEST_CASE(ValDefReworkDeclarationArray)
{
	std::vector<int> _valIntArray{ 9, 12, 7612, 8, 112, 8 };
	std::vector<float> _valFloatArray{ 125.233f, 1.9812f, 89.8612f };
	std::vector<double> _valDoubleArray{ 1.8712, 873.655, 891.87316, 8712.8213 };
	std::vector<bool> _valBoolArray{ true, true, false, true, false, false };

	auto valIntArray = Util::MakeIntArray(_valIntArray);
	auto valFloatArray = Util::MakeFloatArray({ 125.233f, 1.9812f, 89.8612f });
	auto valDoubleArray = Util::MakeDoubleArray({ 1.8712, 873.655, 891.87316, 8712.8213 });
	auto valBoolArray = Util::MakeBoolArray({ true, true, false, true, false, false });

	BOOST_CHECK(Util::IsArray(valIntArray.Type()));
	BOOST_CHECK(Util::IsArray(valFloatArray.Type()));
	BOOST_CHECK(Util::IsArray(valDoubleArray.Type()));
	BOOST_CHECK(Util::IsArray(valBoolArray.Type()));

	BOOST_CHECK_EQUAL(_valIntArray.size(), valIntArray.Type().ArraySize());
	BOOST_CHECK_EQUAL(_valFloatArray.size(), valFloatArray.Type().ArraySize());
	BOOST_CHECK_EQUAL(_valDoubleArray.size(), valDoubleArray.Type().ArraySize());
	BOOST_CHECK_EQUAL(_valBoolArray.size(), valBoolArray.Type().ArraySize());

	BOOST_REQUIRE(_valIntArray == valIntArray.As<std::vector<int>>());
	BOOST_REQUIRE(_valFloatArray == valFloatArray.As<std::vector<float>>());
	BOOST_REQUIRE(_valDoubleArray == valDoubleArray.As<std::vector<double>>());
	BOOST_REQUIRE(_valBoolArray == valBoolArray.As<std::vector<bool>>());
}

BOOST_AUTO_TEST_CASE(ValDefReworkCaptureValue)
{
	int _valInt = 8712;
	float _valFloat = 7812.8612f;
	double _valDouble = 91.72634813;
	char _valChar = 'J';
	bool _valBool = false;

	volatile float _valFloat2 = 7812.8612f;
	const char _valChar2 = 'J';

	auto valInt = Util::MakeAutoValue(std::move(_valInt));
	auto valFloat = Util::MakeAutoValue(std::move(_valFloat));
	auto valDouble = Util::MakeAutoValue(std::move(_valDouble));
	auto valChar = Util::MakeAutoValue(std::move(_valChar));
	auto valBool = Util::MakeAutoValue(std::move(_valBool));

	auto valFloat2 = Util::MakeAutoValue(std::move(_valFloat2));
	auto valChar2 = Util::MakeAutoValue(std::move(_valChar2));

	BOOST_REQUIRE_EQUAL(_valInt, valInt.As<int>());
	BOOST_REQUIRE_EQUAL(_valFloat, valFloat.As<float>());
	BOOST_REQUIRE_EQUAL(_valDouble, valDouble.As<double>());
	BOOST_REQUIRE_EQUAL(_valChar, valChar.As<char>());
	BOOST_REQUIRE_EQUAL(_valBool, valBool.As<bool>());

	BOOST_REQUIRE(Util::IsVolatile(valFloat2.Type()));
	BOOST_REQUIRE(Util::IsConst(valChar2.Type()));
	BOOST_REQUIRE_EQUAL(_valFloat2, valFloat2.As<float>());
	BOOST_REQUIRE_EQUAL(_valChar2, valChar2.As<char>());
}

BOOST_AUTO_TEST_CASE(ValDefReworkCaptureMultiValue)
{
	std::vector<int> _valIntArray{ 12, 89, 761, 86712, 7, 71, 99 };
	std::vector<float> _valFloatArray{ 12.341f, 99.1672f, 1184.812f, 1.7263f };
	std::vector<double> _valDoubleArray{ 923, 1192.23, 7.873123, 9.716289 };
	std::vector<bool> _valBoolArray{ false, false, false, false, true, false, true };

	volatile std::vector<float> _valFloatArray2 = { 9.812f, 87234.21f, 12.1872f, 9.2873f, 7.71628f };
	const std::vector<bool> _valBoolArray2 = { true, true, false, false, true, false, true };

	auto valIntArray = Util::MakeAutoValue(std::move(_valIntArray));
	auto valFloatArray = Util::MakeAutoValue(std::move(_valFloatArray));
	auto valDoubleArray = Util::MakeAutoValue(std::move(_valDoubleArray));
	auto valBoolArray = Util::MakeAutoValue(std::move(_valBoolArray));

	auto valFloatArray2 = Util::MakeAutoValue(std::move(_valFloatArray2));
	auto valBoolArray2 = Util::MakeAutoValue(std::move(_valBoolArray2));

	BOOST_REQUIRE(_valIntArray == valIntArray.As<std::vector<int>>());
	BOOST_REQUIRE(_valFloatArray == valFloatArray.As<std::vector<float>>());
	BOOST_REQUIRE(_valDoubleArray == valDoubleArray.As<std::vector<double>>());
	BOOST_REQUIRE(_valBoolArray == valBoolArray.As<std::vector<bool>>());

	BOOST_REQUIRE(Util::IsVolatile(valFloatArray2.Type()));
	BOOST_REQUIRE(Util::IsConst(valBoolArray2.Type()));
	BOOST_REQUIRE(const_cast<std::vector<float>&>(_valFloatArray2) == valFloatArray2.As<std::vector<float>>());
	BOOST_REQUIRE(_valBoolArray2 == valBoolArray2.As<std::vector<bool>>());
}

BOOST_AUTO_TEST_CASE(ValDefReworkPointer)
{
	{
		auto valInt = Util::MakeInt(1547483642);
		auto valPtr = Util::MakePointer(std::move(valInt));

		BOOST_CHECK(!valPtr.Empty());
		BOOST_REQUIRE(valPtr.IsReference());
		BOOST_REQUIRE_EQUAL(1547483642, valPtr.As<Value>().As<int>());
	}

	{
		std::vector<double> _valDoubleArray{ 923, 1192.23, 7.873123, 9.716289 };
		auto valDoubleArray = Util::MakeAutoValue(std::move(_valDoubleArray));
		auto valPtr = Util::MakeAutoValue(std::move(valDoubleArray));

		BOOST_CHECK(!valPtr.Empty());
		BOOST_REQUIRE(valPtr.IsReference());
		BOOST_REQUIRE(!valPtr.IsArray());
		BOOST_REQUIRE(Util::IsArray(valPtr.As<Value>().Type()));
		BOOST_REQUIRE(_valDoubleArray == valPtr.As<Value>().As<std::vector<double>>());
	}
}

BOOST_AUTO_TEST_CASE(ValDefReworkRecord)
{
	auto valInt = Util::MakeInt(4234761);
	auto valFloatArray = Util::MakeFloatArray({ 125.233f, 1.9812f, 89.8612f });

	RecordValue record;
	record.AddField({ "i", RecordValue::AutoValue(valInt) });
	record.AddField({ "j", RecordValue::AutoValue(valFloatArray) });

	RecordValue record2{ record };

	auto valStruct = Util::MakeStruct(std::move(record));
	BOOST_REQUIRE(Util::IsStruct(valStruct.Type()));

	BOOST_CHECK(!valStruct.Empty());
	BOOST_CHECK(valInt == (*valStruct.As<RecordValue>().GetField("i")));
	BOOST_REQUIRE(record2 == valStruct.As<RecordValue>());
}

BOOST_AUTO_TEST_CASE(ValDefReworkReplace)
{
	{
		auto valInt = Util::MakeInt(982734);
		valInt = Util::MakeInt(17);
		BOOST_REQUIRE_EQUAL(17, valInt.As<int>());
		BOOST_REQUIRE(BuiltinType::Specifier::INT == valInt.Type().DataType<BuiltinType>()->TypeSpecifier());
		valInt = Util::MakeInt(7862138);
		BOOST_REQUIRE(BuiltinType::Specifier::INT == valInt.Type().DataType<BuiltinType>()->TypeSpecifier());
		BOOST_REQUIRE_THROW(valInt = Util::MakeFloat(12.23f), Value::InvalidTypeCastException);
	}

	{
		auto valDouble = Util::MakeAutoValue(8273.87123);
		Value val2 = valDouble;
		BOOST_CHECK(valDouble);
		BOOST_REQUIRE_EQUAL(8273.87123, val2.As<double>());
	}
}

BOOST_AUTO_TEST_CASE(ValDefReworkMisc)
{
	{
		BOOST_REQUIRE(Util::IsIntegral(Util::MakeInt(722).Type()));
		BOOST_REQUIRE(!Util::IsIntegral(Util::MakeFloat(8.851283f).Type()));
		BOOST_REQUIRE(Util::IsFloatingPoint(Util::MakeFloat(8.851283f).Type()));
	}

	{
		BOOST_REQUIRE(Util::EvaluateValueAsBoolean(Util::MakeInt(722)));
		BOOST_REQUIRE(!Util::EvaluateValueAsBoolean(Util::MakeInt(0)));
		BOOST_REQUIRE_EQUAL(762386, Util::EvaluateValueAsInteger(Util::MakeInt(762386)));
		BOOST_REQUIRE_EQUAL(0, Util::EvaluateValueAsInteger(Util::MakeInt(0)));
		BOOST_REQUIRE_THROW(Util::EvaluateValueAsInteger(Util::MakeIntArray({ 12,23 })), Value::UninitializedValueException);
		BOOST_REQUIRE_THROW(Util::EvaluateValueAsInteger(Util::MakeFloat(8.12f)), Value::InvalidTypeCastException);
	}
}

BOOST_AUTO_TEST_CASE(ValDefRecordMemberValue)
{
	// Test if existing value can be accessed via 'RecordMemberValue'.
	{
		auto valInt = Util::MakeInt(7261);
		auto valString = Util::MakeString("teststring");

		RecordValue record;
		record.AddField({ "int", RecordValue::AutoValue(valInt) });
		record.AddField({ "str", RecordValue::AutoValue(valString) });

		auto valUnion = Util::MakeUnion(std::move(record));

		auto valMemInt = RecordMemberValue(valUnion, "int");
		BOOST_REQUIRE(valInt == (*valMemInt));
	}

	// Create record value and member in value.
	{
		auto record = Util::MakeRecordType("union", RecordType::Specifier::UNION);
		record->AddField("int", std::make_shared<BaseType2::element_type>(Util::MakeBuiltinType(BuiltinType::Specifier::INT)));
		record->AddField("char", std::make_shared<BaseType2::element_type>(Util::MakeBuiltinType(BuiltinType::Specifier::CHAR)));

		Value valUninit{ TypeFacade{ record } };

		auto valMemInt = RecordMemberValue(valUninit, "int");
		BOOST_CHECK(valMemInt->Empty());
		(*valMemInt) = Util::MakeInt(17);

		auto valMemInt2 = RecordMemberValue(valUninit, "int");
		BOOST_REQUIRE_EQUAL(Util::MakeInt(17), (*valMemInt2));
	}
}

BOOST_AUTO_TEST_CASE(ValDefReworkSerialize)
{
	using namespace Util;

	{
		const Value val = Util::MakeAutoValue(7962193);
		Cry::ByteArray buffer = val.Serialize();

		const Value val2 = ValueFactory::MakeValue(buffer);
		BOOST_REQUIRE_EQUAL(val, val2);
	}

	{
		const Value val = Util::MakeAutoValue('O');
		Cry::ByteArray buffer = val.Serialize();

		const Value val2 = ValueFactory::MakeValue(buffer);
		BOOST_REQUIRE_EQUAL(val, val2);
	}

	{
		const Value val = MakeIntArray({ 722, 81, 86131, 71 });
		Cry::ByteArray buffer = val.Serialize();

		const Value val2 = ValueFactory::MakeValue(buffer);
		BOOST_REQUIRE_EQUAL(val, val2);
	}

	{
		const Value val = Util::MakeString("teststring");
		Cry::ByteArray buffer = val.Serialize();

		const Value val2 = ValueFactory::MakeValue(buffer);
		BOOST_REQUIRE_EQUAL("teststring", val2.As<std::string>());
	}

	{
		RecordValue record;
		record.AddField({ "o", RecordValue::AutoValue(Util::MakeInt(82371)) });
		record.AddField({ "p", RecordValue::AutoValue(Util::MakeInt(19)) });

		RecordValue record2{ record };

		auto val = Util::MakeStruct(std::move(record));
		Cry::ByteArray buffer = val.Serialize();

		const Value val2 = ValueFactory::MakeValue(buffer);
		BOOST_REQUIRE_EQUAL(val, val2);
	}

	{
		auto valInt = Util::MakeInt(796162);
		auto val = Util::MakePointer(std::move(valInt));
		Cry::ByteArray buffer = val.Serialize();

		const Value val2 = ValueFactory::MakeValue(buffer);
		BOOST_REQUIRE(val2.IsReference());
		BOOST_REQUIRE_EQUAL(796162, val2.As<Value>().As<int>());
	}
}

BOOST_AUTO_TEST_SUITE_END()

#endif // _OBSOLETE_
