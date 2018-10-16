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
// Key         : ValCat
// Test        : Test all operations of the known value categories.
// Type        : unit
// Description : -
//

// Testcases for the following value cateogies:
//   1.) NilValue.
//   2.) PointerValue.
//   3.) ReferenceValue.
//   4.) BuiltinValue.
//   5.) ArrayValue.
//   6.) RecordValue.

using namespace CryCC::SubValue::Valuedef;

BOOST_AUTO_TEST_SUITE(ValCat)

//
// NilValue.
//

BOOST_AUTO_TEST_CASE(ValCatNilValue)
{
	NilValue valNil;

	BOOST_REQUIRE_EQUAL("(nil)", valNil.ToString());
}

BOOST_AUTO_TEST_CASE(ValCatNilValueMisc)
{
	NilValue valNil;
	NilValue valCopy{ valNil };
	NilValue valMove{ std::move(valCopy) };

	BOOST_REQUIRE(valNil == valMove);
}


//
// PointerValue.
//

//BOOST_AUTO_TEST_CASE(ValCatPointerValue)
//{
//	//TODO:
//}

//
// ReferenceValue.
//

//BOOST_AUTO_TEST_CASE(ValCatReferenceValue)
//{
//	//TODO:
//}

//
// BuiltinValue.
//

BOOST_AUTO_TEST_CASE(ValCatBuiltinValue)
{
	BuiltinValue valChar{ 'x' };
	BuiltinValue valShort{ (short)34 };
	BuiltinValue valInt{ 17 };
	BuiltinValue valLong{ 100L };

	BuiltinValue valUchar{ (unsigned char)'Y' };
	BuiltinValue valUshort{ (unsigned short)82 };
	BuiltinValue valUint{ 92U };
	BuiltinValue valUlong{ 100UL };

	BuiltinValue valBool{ true };
	BuiltinValue valFloat{ 692.834f };
	BuiltinValue valDouble{ 97347.862341 };

	BOOST_REQUIRE_EQUAL('x', valChar.As<char>());
	BOOST_REQUIRE_EQUAL((short)34, valShort.As<short>());
	BOOST_REQUIRE_EQUAL(17, valInt.As<int>());
	BOOST_REQUIRE_EQUAL(100L, valLong.As<long>());

	BOOST_REQUIRE_EQUAL((unsigned char)'Y', valUchar.As<unsigned char>());
	BOOST_REQUIRE_EQUAL((unsigned short)82, valUshort.As<unsigned short>());
	BOOST_REQUIRE_EQUAL(92U, valUint.As<unsigned int>());
	BOOST_REQUIRE_EQUAL(100UL, valUlong.As<unsigned long>());

	BOOST_REQUIRE_EQUAL(true, valBool.As<bool>());
	BOOST_REQUIRE_EQUAL(692.834f, valFloat.As<float>());
	BOOST_REQUIRE_EQUAL(97347.862341, valDouble.As<double>());
}

BOOST_AUTO_TEST_CASE(ValCatBuiltinValueArith)
{
	// Addition.
	{
		BuiltinValue valChar{ 'a' };
		BuiltinValue valShort{ (short)34 };
		BuiltinValue valInt{ 81 };
		BuiltinValue valLong{ 8236912L };

		BuiltinValue valAdd = valLong + valChar + valInt + valShort;

		BOOST_REQUIRE_EQUAL(8237124L, valAdd.As<long>());
	}

	// Subtraction.
	{
		BuiltinValue valInt{ 3341 };
		BuiltinValue valShort{ (short)-19 };

		BuiltinValue valSub = valInt - valShort;

		BOOST_REQUIRE_EQUAL(3360L, valSub.As<int>());
	}

	// Multiplication.
	{
		BuiltinValue valFloat{ 23.348f };
		BuiltinValue valLong{ 29L };

		BuiltinValue valSub = valFloat * valLong;

		BOOST_REQUIRE_EQUAL(677.09198f, valSub.As<float>());
	}

	// Division.
	{
		BuiltinValue valUchar{ (unsigned char)'V' };
		BuiltinValue valChar{ (unsigned char)0x4 };

		BuiltinValue valSub = valUchar / valChar;

		BOOST_REQUIRE_EQUAL(21, valSub.As<int>());
	}

	//TODO
	// Modulo.
	/*{
		BuiltinValue valUint{ 82381271U };
		BuiltinValue valInt{ 5 };

		BuiltinValue valSub = valUint % valInt;

		BOOST_REQUIRE_EQUAL(1L, valSub.As<long>());
	}*/

	// Increase/decrease.
	{
		BuiltinValue valInt{ 172441 };
		++valInt;
		valInt++;

		BuiltinValue valUlong{ 8613UL };
		--valUlong;
		valUlong--;

		BOOST_REQUIRE_EQUAL(172443, valInt.As<int>());
		BOOST_REQUIRE_EQUAL(8611UL, valUlong.As<unsigned long>());
	}
}

BOOST_AUTO_TEST_CASE(ValCatBuiltinValueSerialize)
{
	Cry::ByteArray baInt;
	BuiltinValue valInt{ 17 };
	BuiltinValue::Serialize(valInt, baInt);
	BuiltinValue valIntExp{ 0 };
	BuiltinValue::Deserialize(valIntExp, baInt);

	Cry::ByteArray baUint;
	BuiltinValue valUint{ 92U };
	BuiltinValue::Serialize(valUint, baUint);
	BuiltinValue valUintExp{ 0U };
	BuiltinValue::Deserialize(valUintExp, baUint);

	BOOST_REQUIRE_EQUAL(17, valIntExp.As<int>());
	BOOST_REQUIRE_EQUAL(92U, valUintExp.As<unsigned int>());
}

BOOST_AUTO_TEST_CASE(ValCatBuiltinValueMisc)
{
	BuiltinValue valInt{ 17 };
	BuiltinValue valInt2{ 18 };
	BuiltinValue valCopy{ valInt };
	BuiltinValue valMove{ std::move(valCopy) };
	valInt = 18;

	BOOST_REQUIRE_EQUAL("17", valMove.ToString());
	BOOST_REQUIRE_EQUAL(17, valMove.As<int>());
	BOOST_REQUIRE_EQUAL(18, valInt.As<int>());
	BOOST_REQUIRE(valInt == valInt2);
	BOOST_REQUIRE(!(valInt == valMove));
}

//
// ArrayValue.
//

BOOST_AUTO_TEST_CASE(ValCatArrayValue)
{
	ArrayValue valArChar{ 'k','a','a','s' };
	ArrayValue valArShort{ (short)2, (short)4, (short)8, (short)913 };
	ArrayValue valArInt{ 1,2,3,4,5,6,7,8,9,0 };
	ArrayValue valArLong{ 100L,200L,300L };

	ArrayValue valArUchar{ (unsigned char)'Y', (unsigned char)'A', (unsigned char)'-' };
	ArrayValue valArUshort{ (unsigned short)92, (unsigned short)15, (unsigned short)94, (unsigned short)3416 };
	ArrayValue valArUint{ 91U,8U,12U,3713U };
	ArrayValue valArUlong{ 100UL,400UL,900260UL };

	ArrayValue valArBool{ false,true,true,false };
	ArrayValue valArFloat{ 84.8748f, 948.847f };
	ArrayValue valArDouble{ 8734.823123, 891.6418 };

	std::vector<int> vector{ 12, 34,45 };
	ArrayValue valArInt2{ vector.cbegin(), vector.cend() };

	BOOST_REQUIRE((std::vector<char>{'k', 'a', 'a', 's'}) == valArChar.As<char>());
	BOOST_REQUIRE((std::vector<short>{(short)2, (short)4, (short)8, (short)913}) == valArShort.As<short>());
	BOOST_REQUIRE((std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 0}) == valArInt.As<int>());
	BOOST_REQUIRE((std::vector<long>{100L, 200L, 300L}) == valArLong.As<long>());

	BOOST_REQUIRE((std::vector<unsigned char>{(unsigned char)'Y', (unsigned char)'A', (unsigned char)'-'}) == valArUchar.As<unsigned char>());
	BOOST_REQUIRE((std::vector<unsigned short>{(unsigned short)92, (unsigned short)15, (unsigned short)94, (unsigned short)3416}) == valArUshort.As<unsigned short>());
	BOOST_REQUIRE((std::vector<unsigned int>{91U, 8U, 12U, 3713U}) == valArUint.As<unsigned int>());
	BOOST_REQUIRE((std::vector<unsigned long>{100UL, 400UL, 900260UL}) == valArUlong.As<unsigned long>());

	BOOST_REQUIRE((std::vector<bool>{false, true, true, false}) == valArBool.As<bool>());
	BOOST_REQUIRE((std::vector<float>{84.8748f, 948.847f}) == valArFloat.As<float>());
	BOOST_REQUIRE((std::vector<double>{8734.823123, 891.6418}) == valArDouble.As<double>());

	BOOST_REQUIRE((std::vector<int>{12, 34, 45}) == valArInt2.As<int>());
}

BOOST_AUTO_TEST_CASE(ValCatArrayValueSerialize)
{
	Cry::ByteArray baArInt;
	ArrayValue valArInt{ 1,2,3,4,5,6,7,8,9,0 };
	ArrayValue::Serialize(valArInt, baArInt);
	ArrayValue valArIntExp{ 0 };
	ArrayValue::Deserialize(valArIntExp, baArInt);

	Cry::ByteArray baArUint;
	ArrayValue valArUint{ 1U,2U,3U,4U,5U,6U,7U,8U,9U };
	ArrayValue::Serialize(valArUint, baArUint);
	ArrayValue valArUintExp{ 0 };
	ArrayValue::Deserialize(valArUintExp, baArUint);

	BOOST_REQUIRE((std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 0}) == valArIntExp.As<int>());
	BOOST_REQUIRE((std::vector<unsigned int>{1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U}) == valArUintExp.As<unsigned int>());
}

BOOST_AUTO_TEST_CASE(ValCatArrayValueIterable)
{
	ArrayValue valArInt{ 1,2,3,4,5,6,7,8,9,0 };
	valArInt.Emplace(6, 77);
	BOOST_REQUIRE_EQUAL(5, valArInt.At<int>(4));
	BOOST_REQUIRE_EQUAL(77, valArInt.At<int>(6));
}

BOOST_AUTO_TEST_CASE(ValCatArrayValueMisc)
{
	ArrayValue valArInt{ 1,2,3,4,5,6,7,8,9,0 };
	ArrayValue valArInt2{ 11,12,13,14,15,16,17,18,19 };
	ArrayValue valCopy{ valArInt };
	ArrayValue valMove{ std::move(valCopy) };
	valArInt = { 11,12,13,14,15,16,17,18,19 };

	BOOST_REQUIRE((std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 0}) == valMove.As<int>());
	BOOST_REQUIRE((std::vector<int>{11, 12, 13, 14, 15, 16, 17, 18, 19}) == valArInt.As<int>());

	BOOST_REQUIRE(valArInt == valArInt2);
	BOOST_REQUIRE(!(valArInt == valMove));
}

//
// RecordValue.
//

BOOST_AUTO_TEST_CASE(ValCatRecordValue)
{
	// Test record size.
	{
		RecordValue record;
		record.AddField(0, Util::MakeInt(12));

		BOOST_REQUIRE_EQUAL(record.Size(), 1);
	}

	//TODO: FIXME: Rewrite the RecordValue::Compare()

	// Compare records.
	//{
	//	auto valInt = Util::MakeInt2(12);
	//	RecordValue anonRecord;
	//	anonRecord.AddField(0, std::move(valInt));

	//	BOOST_REQUIRE_EQUAL(anonRecord.Size(), 1);

	//	auto valInt2 = Util::MakeInt2(12);
	//	RecordValue anonRecord2;
	//	anonRecord2.AddField(0, std::move(valInt2));
	//	BOOST_REQUIRE(anonRecord == anonRecord2);
	//}

	// Compare inequal records.
	//{
	//	auto valDouble = Util::MakeDouble2(8723.7612);
	//	RecordValue record;
	//	record.AddField(0, std::move(valDouble));

	//	auto valDouble2 = Util::MakeDouble2(81.7213);
	//	RecordValue record2;
	//	record2.AddField(0, std::move(valDouble2));
	//	BOOST_REQUIRE(!(record == record2));
	//}
}

BOOST_AUTO_TEST_CASE(ValCatRecordValueSerialize)
{
	//TODO: FIXME: 

	//{
	//	Cry::ByteArray ba;
	//	RecordValue record;
	//	RecordValue::Serialize(record, ba);

	//	RecordValue record2;
	//	RecordValue::Deserialize(record2, ba);

	//	BOOST_REQUIRE(record == record2);
	//}

	//{
	//	Cry::ByteArray ba;
	//	RecordValue record;
	//	record.AddField(0, std::move(Util::MakeInt2(834)));
	//	record.AddField(1, std::move(Util::MakeChar2('Y')));
	//	record.AddField(2, std::move(Util::MakeInt2(0)));
	//	RecordValue::Serialize(record, ba);

	//	RecordValue record2;
	//	RecordValue::Deserialize(record2, ba);

	//	BOOST_REQUIRE_EQUAL(3, record2.Size());
	//	BOOST_REQUIRE(record == record2);
	//}
}

BOOST_AUTO_TEST_CASE(ValCatRecordValueMisc)
{
	BOOST_REQUIRE_EQUAL(0, RecordValue{}.Size());

	/*{
		auto value = RecordValue::AutoValue(Util::MakeInt(81827));

		RecordValue record;
		record.AddField({ "i", value });

		BOOST_REQUIRE_THROW(record.AddField({ "i", value }), RecordValue::FieldExistException);
	}*/
}

BOOST_AUTO_TEST_SUITE_END()
