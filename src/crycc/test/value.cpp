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
// Key          : Value
// Test         : Value definition unitttest
// Type         : unit
// Description  : Unit test of the value type system. Since the value definition 
//                system is the backbone of the compiler it demands for almost
//                full test coverage.
// Requirements : - Value must be assignable, copyable
//                - Value must be serializable,deserialiable
//                - Value must be replacable or item emplacable
//                - Value must be easy constructable
//

//TODO:
// - Make Bool
// - Offset value
// - Arithmetic?

using namespace CryCC::SubValue::Typedef;
using namespace CryCC::SubValue::Valuedef;

BOOST_AUTO_TEST_SUITE(ValueDefinition)

BOOST_AUTO_TEST_CASE(ValueReworkDissected)
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

BOOST_AUTO_TEST_CASE(ValueReworkDeclaration)
{
	auto valInt = Util::MakeInt2(12);
	auto valFloat = Util::MakeFloat2(92.123f);
	auto valDouble = Util::MakeDouble2(87341.78263);
	auto valChar = Util::MakeChar2('K');
	//auto valBool = Util::MakeBool(true);
	auto valULong = Util::MakeUnsignedLong(8273ULL);
	auto valStr = Util::MakeString2("teststring");

	BOOST_CHECK(valInt.Initialized());
	BOOST_CHECK(valFloat.Initialized());
	BOOST_CHECK(valDouble.Initialized());
	BOOST_CHECK(valChar.Initialized());
	BOOST_CHECK(valULong.Initialized());
	//BOOST_CHECK(!valBool.Initialized());
	BOOST_CHECK(valStr.Initialized());

	BOOST_REQUIRE_EQUAL(12, Util::ValueCastNative<int>(valInt));
	BOOST_REQUIRE_EQUAL(92.123f, Util::ValueCastNative<float>(valFloat));
	BOOST_REQUIRE_EQUAL(87341.78263, Util::ValueCastNative<double>(valDouble));
	BOOST_REQUIRE_EQUAL('K', Util::ValueCastNative<char>(valChar));
	BOOST_REQUIRE_EQUAL(8273ULL, Util::ValueCastNative<unsigned long>(valULong));
	//BOOST_REQUIRE_EQUAL(true, valBool.As<BuiltinValue, bool>());
	BOOST_REQUIRE_EQUAL("teststring", Util::ValueCastString(valStr));
}

BOOST_AUTO_TEST_CASE(ValueReworkDeclarationArray)
{
	std::vector<int> _valIntArray{ 9, 12, 7612, 8, 112, 8 };
	std::vector<float> _valFloatArray{ 125.233f, 1.9812f, 89.8612f };
	std::vector<double> _valDoubleArray{ 1.8712, 873.655, 891.87316, 8712.8213 };
	std::vector<bool> _valBoolArray{ true, true, false, true, false, false };

	auto valIntArray = Util::MakeIntArray2(_valIntArray);
	auto valFloatArray = Util::MakeFloatArray2(_valFloatArray);
	auto valDoubleArray = Util::MakeDoubleArray2(_valDoubleArray);
	//auto valBoolArray = Util::MakeBoolArray({ true, true, false, true, false, false });

	BOOST_CHECK(!Util::MultiElementEmpty(valIntArray));
	BOOST_CHECK(!Util::MultiElementEmpty(valFloatArray));
	BOOST_CHECK(!Util::MultiElementEmpty(valDoubleArray));
	//BOOST_CHECK(!Util::MultiElementEmpty(valBoolArray));

	BOOST_CHECK_EQUAL(_valIntArray.size(), Util::MultiElementSize(valIntArray));
	BOOST_CHECK_EQUAL(_valFloatArray.size(), Util::MultiElementSize(valFloatArray));
	BOOST_CHECK_EQUAL(_valDoubleArray.size(), Util::MultiElementSize(valDoubleArray));
	//BOOST_CHECK_EQUAL(_valBoolArray.size(), Util::MultiElementSize(valBoolArray));

	BOOST_REQUIRE(_valIntArray == Util::ValueCastArray<int>(valIntArray));
	BOOST_REQUIRE(_valFloatArray == Util::ValueCastArray<float>(valFloatArray));
	BOOST_REQUIRE(_valDoubleArray == Util::ValueCastArray<double>(valDoubleArray));
	//BOOST_REQUIRE(_valBoolArray == Util::ValueCastArray<bool>(valBoolArray));

	BOOST_REQUIRE_EQUAL(12, (Util::MultiElementAt<int, 1>(valIntArray)));
	BOOST_REQUIRE_EQUAL(89.8612f, (Util::MultiElementAt<float, 2>(valFloatArray)));
	BOOST_REQUIRE_EQUAL(891.87316, (Util::MultiElementAt<double, 2>(valDoubleArray)));
	//BOOST_REQUIRE_EQUAL(false, (Util::MultiElementAt<bool, 4>(valBoolArray)));
}

BOOST_AUTO_TEST_CASE(ValueReworkRecord)
{
	// Make struct with values.
	{
		auto valInt = Util::MakeInt2(4234761);
		auto valFloatArray = Util::MakeFloatArray2({ 125.233f, 1.9812f, 89.8612f });
		auto valDoubleArray = Util::MakeDoubleArray2({ 1.8712, 873.655, 891.87316, 8712.8213 });

		auto valStruct = Util::MakeStruct2("struct", valInt, valFloatArray);
		BOOST_REQUIRE(Util::IsStruct(valStruct));

		BOOST_CHECK_EQUAL(2, valStruct.As<RecordValue>()->Size());

		BOOST_REQUIRE(valStruct);
		BOOST_REQUIRE(valInt == (valStruct.As<RecordValue>()->At(0)));
	}

	// Make union with values.
	{
		auto valInt = Util::MakeInt2(1);
		auto valInt2 = Util::MakeInt2(-29);
		auto valUnion = Util::MakeUnion2("", valInt, valInt2);
		BOOST_REQUIRE(Util::IsUnion(valUnion));
	}
}

BOOST_AUTO_TEST_CASE(ValueReworkReplace)
{
	// Replace builtin integer.
	{
		auto valInt = Util::MakeInt2(982734);
		valInt = Util::MakeInt2(17);
		BOOST_REQUIRE_EQUAL(17, Util::ValueCastNative<int>(valInt));
		valInt = Util::MakeInt2(7862138);
		BOOST_REQUIRE_THROW(valInt = Util::MakeFloat2(12.23f), InvalidTypeCastException);
	}

	// Replace builtin double.
	{
		auto valDouble = Util::MakeDouble2(8273.87123);
		Value2 val2 = valDouble;
		BOOST_CHECK(valDouble);
		BOOST_REQUIRE_EQUAL(8273.87123, Util::ValueCastNative<double>(valDouble));
	}

	// Replace array item.
	{
		auto valIntArray = Util::MakeIntArray2({ 9,81,74,71,613,73 });
		valIntArray = Util::MakeIntArray2({ 23,45,67 });
		BOOST_REQUIRE_EQUAL(23, (Util::MultiElementAt<int, 0>(valIntArray)));
		Util::MultiElementEmplace<1>(valIntArray, 8361);
		BOOST_REQUIRE_EQUAL(8361, (Util::MultiElementAt<int, 1>(valIntArray)));
	}

	// Replace record item.
	{
		auto valInt = Util::MakeInt2(4234761);
		auto valChar = Util::MakeChar2('O');
		auto valChar2 = Util::MakeChar2('Y');

		auto valCharCheck = Value2{ valChar2 };
		auto valStruct = Util::MakeStruct2("struct", valInt, valChar);

		BOOST_REQUIRE(valChar == (valStruct.As<RecordValue>()->At(1)));
		valStruct.As<RecordValue>()->Emplace(1, std::move(valChar2));
		BOOST_REQUIRE(valCharCheck == (valStruct.As<RecordValue>()->At(1)));
	}
}

BOOST_AUTO_TEST_CASE(ValueReworkMisc)
{
	// Unitialized values.
	{
		auto valNil = Util::MakeUninitialized();

		BOOST_REQUIRE(!valNil.Initialized());
		BOOST_REQUIRE(!valNil);
	}

	// Integer value helpers.
	{
		BOOST_REQUIRE(Util::IsIntegral(Util::MakeInt2(722)));
		BOOST_REQUIRE(!Util::IsIntegral(Util::MakeFloat2(8.851283f)));
		BOOST_REQUIRE(Util::IsSigned(Util::MakeInt2(-82361)));
		BOOST_REQUIRE(!Util::IsUnsigned(Util::MakeUnsignedInt(928)));
		BOOST_REQUIRE(Util::IsFloatingPoint(Util::MakeFloat2(8.851283f)));
	}

	// Value evaluation.
	{
		BOOST_REQUIRE(Util::EvaluateValueAsBoolean(Util::MakeInt2(722)));
		BOOST_REQUIRE(!Util::EvaluateValueAsBoolean(Util::MakeInt2(0)));
		BOOST_REQUIRE_EQUAL(762386, Util::EvaluateValueAsInteger(Util::MakeInt2(762386)));
		BOOST_REQUIRE_EQUAL(0, Util::EvaluateValueAsInteger(Util::MakeInt2(0)));
		//BOOST_REQUIRE_THROW(Util::EvaluateValueAsInteger(Util::MakeIntArray2({ 12,23 })), UninitializedValueException);
		BOOST_REQUIRE_THROW(Util::EvaluateValueAsInteger(Util::MakeFloat2(8.12f)), InvalidTypeCastException);
	}

	// Value string streaming.
	{
		auto valInt = Util::MakeInt2(9471);

		std::stringstream ss;
		ss << valInt;
		BOOST_REQUIRE_EQUAL("9471", ss.str());
	}
}

BOOST_AUTO_TEST_CASE(ValueReworkSerialize)
{
	using namespace Util;

	// Serialize nil value.
	{
		const Value2 val = Util::MakeUninitialized();

		Cry::ByteArray buffer;
		Value2::Serialize(val, buffer);

		Value2 val2 = Util::MakeInt2(1);
		Value2::Deserialize(val2, buffer);

		BOOST_REQUIRE_EQUAL(val, val2);
	}

	// Serialize builtin int.
	{
		const Value2 val = Util::MakeInt2(10);

		Cry::ByteArray buffer;
		Value2::Serialize(val, buffer);

		Value2 val2 = MakeUninitialized();
		Value2::Deserialize(val2, buffer);

		BOOST_REQUIRE_EQUAL(val, val2);
	}

	// Serialize builtin double.
	{
		const Value2 val = Util::MakeDouble2(23.34143);

		Cry::ByteArray buffer;
		Value2::Serialize(val, buffer);

		Value2 val2 = MakeUninitialized();
		Value2::Deserialize(val2, buffer);

		//BOOST_REQUIRE_EQUAL(val, val2);
	}

	// Serialize array with integers.
	{
		const Value2 val = Util::MakeIntArray2({ 722, 81, 86131, 71 });

		Cry::ByteArray buffer;
		Value2::Serialize(val, buffer);

		Value2 val2 = MakeUninitialized();
		Value2::Deserialize(val2, buffer);

		BOOST_REQUIRE_EQUAL(val, val2);
	}

	// Serialize array composed string.
	//{
	//	const Value2 val = Util::MakeString2("teststring");

	//	Cry::ByteArray buffer;
	//	Value2::Serialize(val, buffer);

	//	Value2 val2 = MakeUninitialized();
	//	Value2::Deserialize(val2, buffer);

	//	BOOST_REQUIRE_EQUAL(val, val2);
	//}

	// Serialize record.
	{
		const Value2 valInt = Util::MakeInt2(1);
		const Value2 valInt2 = Util::MakeInt2(-29);
		const Value2 val = Util::MakeUnion2("", valInt, valInt2);

		Cry::ByteArray buffer;
		Value2::Serialize(val, buffer);

		Value2 val2 = MakeUninitialized();
		Value2::Deserialize(val2, buffer);

		BOOST_REQUIRE_EQUAL(val, val2);
	}

	//TODO:
	// - Serialize ReferenceValue
	// - Serialize PointerValue
}

BOOST_AUTO_TEST_SUITE_END()
