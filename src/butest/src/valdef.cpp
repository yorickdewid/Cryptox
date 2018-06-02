// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "../src/Valuedef.h"

#include <boost/test/unit_test.hpp>

//
// Test        : Value definition unitttest
// Type        : unit
// Description : -
//

using namespace CoilCl;

BOOST_AUTO_TEST_SUITE(ValueDefinition)

BOOST_AUTO_TEST_CASE(ValDefLegacyBasic)
{
	//NOTE: Float and string fail

	auto valInt = Util::MakeInt(12);
	auto valDouble = Util::MakeDouble(7123.7263812);
	auto valChar = Util::MakeChar('x');
	auto valBool = Util::MakeBool(true);
	auto valVoid = Util::MakeVoid();

	BOOST_CHECK(!Util::IsValueArray(valInt));
	BOOST_CHECK(!Util::IsValueArray(valDouble));
	BOOST_CHECK(!Util::IsValueArray(valChar));
	BOOST_CHECK(!Util::IsValueArray(valBool));

	BOOST_REQUIRE_EQUAL(12, valInt->As<int>());
	BOOST_REQUIRE_EQUAL(7123.7263812, valDouble->As<double>());
	BOOST_REQUIRE_EQUAL('x', valChar->As<char>());
	BOOST_REQUIRE_EQUAL(true, valBool->As<bool>());

	BOOST_CHECK(Util::IsValueVoid(valVoid));
}

BOOST_AUTO_TEST_CASE(ValDefLegacyReplace)
{
	{
		auto valInt = Util::MakeInt(37642813);
		valInt->ReplaceValue(812);
		BOOST_REQUIRE_EQUAL(812, valInt->As<int>());
	}

	{
		auto valChar = Util::MakeChar('P');
		auto valChar2 = Util::MakeChar('X');
		valChar->ReplaceValueWithValue(*std::dynamic_pointer_cast<Valuedef::Value>(valChar2));
		BOOST_REQUIRE_EQUAL('X', valChar->As<char>());
	}
}

BOOST_AUTO_TEST_CASE(ValDefBasicReworkDissected)
{
	static_assert(std::is_copy_constructible<Valuedef::Value>::value, "Valuedef::Value lacks special member function: copy");
	static_assert(std::is_move_constructible<Valuedef::Value>::value, "Valuedef::Value lacks special member function: move");

	{
		auto builtin = Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::INT);
		AST::TypeFacade facade{ builtin };
		Valuedef::Value val{ 0, facade, Valuedef::Value::ValueVariant2{ 8612 } };

		BOOST_CHECK(!val.Empty());
		BOOST_REQUIRE_EQUAL(8612, val.As2<int>());
	}

	{
		auto builtin = Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::INT);
		Valuedef::Value val{ 0, AST::TypeFacade{ builtin }, Valuedef::Value::ValueVariant2{ 919261 } };

		BOOST_CHECK(!val.Empty());
		BOOST_REQUIRE_NO_THROW(val.As2<int>());
		BOOST_CHECK_THROW(val.As2<char>(), Valuedef::Value::InvalidTypeCastException);
	}

	{
		auto builtin = Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::SHORT);
		Valuedef::Value val{ 0, AST::TypeFacade{ builtin } };

		BOOST_CHECK(val.Empty());
		BOOST_CHECK_THROW(val.As2<char>(), Valuedef::Value::UninitializedValueException);
	}

	{
		std::vector<int> a{ 8612, 812, 2383, 96, 12 };

		Valuedef::Value val{ 0
			, AST::TypeFacade{ Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::INT) }
			, Valuedef::Value::ValueVariant3{ a } };

		BOOST_CHECK(!val.Empty());
		BOOST_CHECK(val.IsArray());

		std::vector<int> b = val.As2<std::vector<int>>();

		BOOST_REQUIRE(a == b);
	}

	{
		std::vector<char> a{ 'X', 'O', 'A', 'N', 'B' };

		Valuedef::Value val{ 0
			, AST::TypeFacade{ Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::CHAR) }
			, Valuedef::Value::ValueVariant3{ a } };

		BOOST_CHECK(!val.Empty());
		BOOST_CHECK(val.IsArray());

		std::vector<char> b = val.As2<std::vector<char>>();

		BOOST_REQUIRE(a == b);
	}
}

BOOST_AUTO_TEST_CASE(ValDefReworkDeclaration)
{
	auto valStr = Util::MakeString2("teststring");
	auto valInt = Util::MakeInt2(12);
	auto valFloat = Util::MakeFloat2(92.123f);
	auto valDouble = Util::MakeDouble2(87341);
	auto valChar = Util::MakeChar2('K');
	auto valBool = Util::MakeBool2(true);

	BOOST_CHECK(!valStr.Empty());
	BOOST_CHECK(!valInt.Empty());
	BOOST_CHECK(!valFloat.Empty());
	BOOST_CHECK(!valDouble.Empty());
	BOOST_CHECK(!valChar.Empty());
	BOOST_CHECK(!valBool.Empty());

	BOOST_REQUIRE_EQUAL("teststring", valStr.As2<std::string>());
	BOOST_REQUIRE_EQUAL(12, valInt.As2<int>());
	BOOST_REQUIRE_EQUAL(92.123f, valFloat.As2<float>());
	BOOST_REQUIRE_EQUAL(87341, valDouble.As2<double>());
	BOOST_REQUIRE_EQUAL('K', valChar.As2<char>());
	BOOST_REQUIRE_EQUAL(true, valBool.As2<bool>());
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

	BOOST_CHECK(valIntArray.IsArray());
	BOOST_CHECK(valFloatArray.IsArray());
	BOOST_CHECK(valDoubleArray.IsArray());
	BOOST_CHECK(valBoolArray.IsArray());

	BOOST_REQUIRE(_valIntArray == valIntArray.As2<std::vector<int>>());
	BOOST_REQUIRE(_valFloatArray == valFloatArray.As2<std::vector<float>>());
	BOOST_REQUIRE(_valDoubleArray == valDoubleArray.As2<std::vector<double>>());
	BOOST_REQUIRE(_valBoolArray == valBoolArray.As2<std::vector<bool>>());
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

	BOOST_REQUIRE_EQUAL(_valInt, valInt.As2<int>());
	BOOST_REQUIRE_EQUAL(_valFloat, valFloat.As2<float>());
	BOOST_REQUIRE_EQUAL(_valDouble, valDouble.As2<double>());
	BOOST_REQUIRE_EQUAL(_valChar, valChar.As2<char>());
	BOOST_REQUIRE_EQUAL(_valBool, valBool.As2<bool>());

	BOOST_REQUIRE(Util::IsTypeVolatile(valFloat2.Type()));
	BOOST_REQUIRE(Util::IsTypeConst(valChar2.Type()));
	BOOST_REQUIRE_EQUAL(_valFloat2, valFloat2.As2<float>());
	BOOST_REQUIRE_EQUAL(_valChar2, valChar2.As2<char>());
}

BOOST_AUTO_TEST_CASE(ValDefReworkCaptureMultiValue)
{
	//
}

BOOST_AUTO_TEST_SUITE_END()
