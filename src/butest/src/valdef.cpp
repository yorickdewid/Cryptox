// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "../src/Valuedef.h"
#include "../src/ValueHelper.h"

#include <boost/test/unit_test.hpp>

//
// Key         : ValDef
// Test        : Value definition unitttest
// Type        : unit
// Description : Unit test of the value type system. Since the value definition 
//               system is the backbone of the compiler it demands for almost
//               full test coverage.
//

using namespace CoilCl;

BOOST_AUTO_TEST_SUITE(ValueDefinition)

BOOST_AUTO_TEST_CASE(ValDefBasicReworkDissected)
{
	{
		auto builtin = Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::INT);
		Typedef::TypeFacade facade{ builtin };
		Valuedef::Value val{ facade, Valuedef::Value::ValueVariantSingle{ 8612 } };

		BOOST_CHECK(!val.Empty());
		BOOST_REQUIRE_EQUAL(8612, val.As<int>());
	}

	{
		auto builtin = Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::INT);
		Valuedef::Value val{ Typedef::TypeFacade{ builtin }, Valuedef::Value::ValueVariantSingle{ 919261 } };

		BOOST_CHECK(!val.Empty());
		BOOST_REQUIRE_NO_THROW(val.As<int>());
		BOOST_CHECK_THROW(val.As<char>(), Valuedef::Value::InvalidTypeCastException);
	}

	{
		auto builtin = Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::SHORT);
		Valuedef::Value val{ Typedef::TypeFacade{ builtin } };

		BOOST_CHECK(!val);
		BOOST_CHECK(val.Empty());
		BOOST_CHECK_THROW(val.As<char>(), Valuedef::Value::UninitializedValueException);
	}

	{
		std::vector<int> a{ 8612, 812, 2383, 96, 12 };

		Valuedef::Value val{Typedef::TypeFacade{ Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::INT) }
			, Valuedef::Value::ValueVariantMulti{ a } };

		BOOST_CHECK(!val.Empty());
		BOOST_CHECK(Util::IsArray(val.Type()));

		std::vector<int> b = val.As<std::vector<int>>();

		BOOST_REQUIRE(a == b);
	}

	{
		std::vector<char> a{ 'X', 'O', 'A', 'N', 'B' };

		Valuedef::Value val{Typedef::TypeFacade{ Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::CHAR) }
			, Valuedef::Value::ValueVariantMulti{ a } };

		BOOST_CHECK(!val.Empty());
		BOOST_CHECK(Util::IsArray(val.Type()));

		std::vector<char> b = val.As<std::vector<char>>();

		BOOST_REQUIRE(a == b);
	}

	{
		Valuedef::Value val1{Typedef::TypeFacade{ Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::CHAR) }
			, Valuedef::Value::ValueVariantSingle{ 19 } };

		BOOST_CHECK(val1);

		Valuedef::Value val2{Typedef::TypeFacade{ Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::CHAR) }
			, Valuedef::Value::ValueVariantSingle{ 12 } };

		BOOST_CHECK(val2);

		Valuedef::Value val3{Typedef::TypeFacade{ Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::CHAR) }
			, Valuedef::Value::ValueVariantSingle{ 19 } };

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

	auto valInt = CaptureValue(_valInt);
	auto valFloat = CaptureValue(_valFloat);
	auto valDouble = CaptureValue(_valDouble);
	auto valChar = CaptureValue(_valChar);
	auto valBool = CaptureValue(_valBool);

	auto valFloat2 = CaptureValue(_valFloat2);
	auto valChar2 = CaptureValue(_valChar2);

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

	auto valIntArray = CaptureValue(_valIntArray);
	auto valFloatArray = CaptureValue(_valFloatArray);
	auto valDoubleArray = CaptureValue(_valDoubleArray);
	auto valBoolArray = CaptureValue(_valBoolArray);

	auto valFloatArray2 = CaptureValue(_valFloatArray2);
	auto valBoolArray2 = CaptureValue(_valBoolArray2);

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
		BOOST_REQUIRE_EQUAL(1547483642, valPtr.As<Valuedef::Value>().As<int>());
	}

	{
		std::vector<double> _valDoubleArray{ 923, 1192.23, 7.873123, 9.716289 };
		auto valDoubleArray = CaptureValue(_valDoubleArray);
		auto valPtr = CaptureValue(valDoubleArray);

		BOOST_CHECK(!valPtr.Empty());
		BOOST_REQUIRE(valPtr.IsReference());
		BOOST_REQUIRE(!valPtr.IsArray());
		BOOST_REQUIRE(Util::IsArray(valPtr.As<Valuedef::Value>().Type()));
		BOOST_REQUIRE(_valDoubleArray == valPtr.As<Valuedef::Value>().As<std::vector<double>>());
	}
}

BOOST_AUTO_TEST_CASE(ValDefReworkRecord)
{
	auto valInt = Util::MakeInt(4234761);
	auto valFloatArray = Util::MakeFloatArray({ 125.233f, 1.9812f, 89.8612f });

	Valuedef::RecordValue record{ "somestruct" };
	record.AddField({ "i", Valuedef::RecordValue::AutoValue(valInt) });
	record.AddField({ "j", Valuedef::RecordValue::AutoValue(valFloatArray) });

	Valuedef::RecordValue record2{ record };

	auto valStruct = Util::MakeStruct(std::move(record));
	BOOST_REQUIRE(Util::IsStruct(valStruct.Type()));

	BOOST_CHECK(!valStruct.Empty());
	BOOST_REQUIRE_EQUAL(record2, valStruct.As<Valuedef::RecordValue>());
}

BOOST_AUTO_TEST_CASE(ValDefReworkReplace)
{
	{
		auto valInt = Util::MakeInt(982734);
		valInt = Util::MakeInt(17);
		BOOST_REQUIRE_EQUAL(17, valInt.As<int>());
		BOOST_REQUIRE(Typedef::BuiltinType::Specifier::INT == valInt.Type().DataType<Typedef::BuiltinType>()->TypeSpecifier());
		valInt = Util::MakeFloat(12.821639f);
		BOOST_REQUIRE(Typedef::BuiltinType::Specifier::FLOAT == valInt.Type().DataType<Typedef::BuiltinType>()->TypeSpecifier());
	}

	{
		auto valDouble = CaptureValue(8273.87123);
		Valuedef::Value val2 = valDouble;
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
		BOOST_REQUIRE_THROW(Util::EvaluateValueAsInteger(Util::MakeIntArray({ 12,23 })), Valuedef::Value::UninitializedValueException);
		BOOST_REQUIRE_THROW(Util::EvaluateValueAsInteger(Util::MakeFloat(8.12f)), Valuedef::Value::InvalidTypeCastException);
	}
}

BOOST_AUTO_TEST_CASE(ValDefReworkSerialize)
{
	using namespace Valuedef;
	using namespace Util;

	{
		const Value val = CaptureValue(7962193);
		Cry::ByteArray buffer = val.Serialize();

		const Value val2 = ValueFactory::MakeValue(buffer);
		BOOST_REQUIRE_EQUAL(val, val2);
	}

	{
		const Value val = CaptureValue('O');
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
		Valuedef::RecordValue record{ "struct" };
		record.AddField({ "o", Valuedef::RecordValue::AutoValue(Util::MakeInt(82371)) });
		record.AddField({ "p", Valuedef::RecordValue::AutoValue(Util::MakeInt(19)) });

		Valuedef::RecordValue record2{ record };

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
		BOOST_REQUIRE_EQUAL(796162, val2.As<Valuedef::Value>().As<int>());
	}
}

BOOST_AUTO_TEST_SUITE_END()
