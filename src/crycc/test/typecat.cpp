// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

//#include <CryCC/SubValue.h>
#include <CryCC/SubValue/NilType.h>
#include <CryCC/SubValue/BuiltinType.h>

#include <boost/test/unit_test.hpp>

//
// Key         : TypeCat
// Test        : Test all operations of the known types categories.
// Type        : unit
// Description : -
//

// Testcases for the following type cateogies:
//   1.) NilType.
//   2.) VariantType.
//   3.) TypedefType.
//   4.) BuiltinType.
//   5.) ArrayType.
//   6.) RecordType.
//   7.) VariadicType.

using namespace CryCC::SubValue::Typedef;

BOOST_AUTO_TEST_SUITE(TypeCat)

//
// NilType.
//

BOOST_AUTO_TEST_CASE(TypeCatNilType)
{
	NilType tyNil;

	BOOST_REQUIRE_EQUAL("(nil)", tyNil.ToString());
	BOOST_REQUIRE_EQUAL(0, tyNil.UnboxedSize());
	BOOST_REQUIRE(TypeVariation::NIL == tyNil.TypeId());
}

BOOST_AUTO_TEST_CASE(TypeCatNilTypeSerialize)
{
	using namespace Cry::ByteStream;

	VectorStream veType;
	NilType tyNil;
	tyNil.SetQualifier(NilType::TypeQualifier::CONST_T);
	tyNil.SetInline();
	tyNil.SetSensitive();
	NilType::Serialize(tyNil, veType);
	NilType tyNilExp;
	NilType::Deserialize(tyNilExp, veType);

	BOOST_REQUIRE(tyNilExp == tyNil);
}

BOOST_AUTO_TEST_CASE(TypeCatNilTypeMisc)
{
	NilType tyNil;
	//	NilType tyCopy{ tyNil };
	//	NilType tyMove{ std::move(tyCopy) };
	//
	//	BOOST_REQUIRE(tyNil == tyMove);
}

//
// VariantType.
//

BOOST_AUTO_TEST_CASE(TypeCatVariantType)
{
	//TODO:
}

BOOST_AUTO_TEST_CASE(TypeCatVariantTypeSerialize)
{
	//TODO:
}

BOOST_AUTO_TEST_CASE(TypeCatVariantTypeMisc)
{
	//TODO:
}

//
// TypedefType.
//

BOOST_AUTO_TEST_CASE(TypeCatTypedefType)
{
	//TODO:
}

BOOST_AUTO_TEST_CASE(TypeCatTypedefTypeSerialize)
{
	//TODO:
}

BOOST_AUTO_TEST_CASE(TypeCatTypedefTypeMisc)
{
	//TODO:
}

//
// BuiltinType.
//

BOOST_AUTO_TEST_CASE(TypeCatBuiltinType)
{
	BuiltinType tyVoid{ BuiltinType::Specifier::VOID_T };
	BuiltinType tyBool{ BuiltinType::Specifier::BOOL_T };
	BuiltinType tyChar{ BuiltinType::Specifier::CHAR_T };
	BuiltinType tySChar{ BuiltinType::Specifier::SIGNED_CHAR_T };
	BuiltinType tyUChar{ BuiltinType::Specifier::UNSIGNED_CHAR_T };
	BuiltinType tyShort{ BuiltinType::Specifier::SHORT_T };
	BuiltinType tyUShort{ BuiltinType::Specifier::UNSIGNED_SHORT_T };
	BuiltinType tyInt{ BuiltinType::Specifier::INT_T };
	BuiltinType tyUInt{ BuiltinType::Specifier::UNSIGNED_INT_T };
	BuiltinType tyLong{ BuiltinType::Specifier::LONG_T };
	BuiltinType tyULong{ BuiltinType::Specifier::UNSIGNED_LONG_T };
	BuiltinType tyFloat{ BuiltinType::Specifier::FLOAT_T };
	BuiltinType tyDouble{ BuiltinType::Specifier::DOUBLE_T };
	BuiltinType tyLDouble{ BuiltinType::Specifier::LONG_DOUBLE_T };

	BOOST_REQUIRE(!tyChar.Unsigned());
	BOOST_REQUIRE(!tySChar.Unsigned());
	BOOST_REQUIRE(tyUChar.Unsigned());
	BOOST_REQUIRE(!tyShort.Unsigned());
	BOOST_REQUIRE(tyUShort.Unsigned());
	BOOST_REQUIRE(!tyInt.Unsigned());
	BOOST_REQUIRE(tyUInt.Unsigned());
	BOOST_REQUIRE(!tyLong.Unsigned());
	BOOST_REQUIRE(tyULong.Unsigned());

	BOOST_REQUIRE(tyChar.Signed());
	BOOST_REQUIRE(tySChar.Signed());
	BOOST_REQUIRE(!tyUChar.Signed());
	BOOST_REQUIRE(tyShort.Signed());
	BOOST_REQUIRE(!tyUShort.Signed());
	BOOST_REQUIRE(tyInt.Signed());
	BOOST_REQUIRE(!tyUInt.Signed());
	BOOST_REQUIRE(tyLong.Signed());
	BOOST_REQUIRE(!tyULong.Signed());
}

BOOST_AUTO_TEST_CASE(TypeCatBuiltinTypeSerialize)
{
	using namespace Cry::ByteStream;

	{
		VectorStream veType;
		BuiltinType tyInt{ BuiltinType::Specifier::INT_T };
		BuiltinType::Serialize(tyInt, veType);
		BuiltinType tyIntExp{ BuiltinType::Specifier::INT_T };
		BuiltinType::Deserialize(tyIntExp, veType);

		BOOST_REQUIRE(tyIntExp == tyInt);
	}

	{
		VectorStream veType;
		BuiltinType tyUint{ BuiltinType::Specifier::UNSIGNED_INT_T };
		BuiltinType::Serialize(tyUint, veType);
		BuiltinType tyUintExp{ BuiltinType::Specifier::UNSIGNED_INT_T };
		BuiltinType::Deserialize(tyUintExp, veType);

		BOOST_REQUIRE(tyUintExp == tyUint);
	}
}

BOOST_AUTO_TEST_CASE(TypeCatBuiltinTypeMisc)
{
	//	BuiltinValue valInt{ 17 };
	//	BuiltinValue valInt2{ 18 };
	//	BuiltinValue valCopy{ valInt };
	//	BuiltinValue valMove{ std::move(valCopy) };
	//	valInt = 18;
	//
	//	BOOST_REQUIRE_EQUAL("17", valMove.ToString());
	//	BOOST_REQUIRE_EQUAL(17, valMove.As<int>());
	//	BOOST_REQUIRE_EQUAL(18, valInt.As<int>());
	//	BOOST_REQUIRE(valInt == valInt2);
	//	BOOST_REQUIRE(!(valInt == valMove));
}

//
// ArrayType.
//

BOOST_AUTO_TEST_CASE(TypeCatArrayType)
{
	//	ArrayValue valArChar{ 'k','a','a','s' };
	//	ArrayValue valArShort{ (short)2, (short)4, (short)8, (short)913 };
	//	ArrayValue valArInt{ 1,2,3,4,5,6,7,8,9,0 };
	//	ArrayValue valArLong{ 100L,200L,300L };
	//
	//	ArrayValue valArUchar{ (unsigned char)'Y', (unsigned char)'A', (unsigned char)'-' };
	//	ArrayValue valArUshort{ (unsigned short)92, (unsigned short)15, (unsigned short)94, (unsigned short)3416 };
	//	ArrayValue valArUint{ 91U,8U,12U,3713U };
	//	ArrayValue valArUlong{ 100UL,400UL,900260UL };
	//
	//	ArrayValue valArBool{ false,true,true,false };
	//	ArrayValue valArFloat{ 84.8748f, 948.847f };
	//	ArrayValue valArDouble{ 8734.823123, 891.6418 };
	//
	//	std::vector<int> vector{ 12, 34,45 };
	//	ArrayValue valArInt2{ vector.cbegin(), vector.cend() };
	//
	//	BOOST_REQUIRE((std::vector<char>{'k', 'a', 'a', 's'}) == valArChar.As<char>());
	//	BOOST_REQUIRE((std::vector<short>{(short)2, (short)4, (short)8, (short)913}) == valArShort.As<short>());
	//	BOOST_REQUIRE((std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 0}) == valArInt.As<int>());
	//	BOOST_REQUIRE((std::vector<long>{100L, 200L, 300L}) == valArLong.As<long>());
	//
	//	BOOST_REQUIRE((std::vector<unsigned char>{(unsigned char)'Y', (unsigned char)'A', (unsigned char)'-'}) == valArUchar.As<unsigned char>());
	//	BOOST_REQUIRE((std::vector<unsigned short>{(unsigned short)92, (unsigned short)15, (unsigned short)94, (unsigned short)3416}) == valArUshort.As<unsigned short>());
	//	BOOST_REQUIRE((std::vector<unsigned int>{91U, 8U, 12U, 3713U}) == valArUint.As<unsigned int>());
	//	BOOST_REQUIRE((std::vector<unsigned long>{100UL, 400UL, 900260UL}) == valArUlong.As<unsigned long>());
	//
	//	BOOST_REQUIRE((std::vector<bool>{false, true, true, false}) == valArBool.As<bool>());
	//	BOOST_REQUIRE((std::vector<float>{84.8748f, 948.847f}) == valArFloat.As<float>());
	//	BOOST_REQUIRE((std::vector<double>{8734.823123, 891.6418}) == valArDouble.As<double>());
	//
	//	BOOST_REQUIRE((std::vector<int>{12, 34, 45}) == valArInt2.As<int>());
}

BOOST_AUTO_TEST_CASE(TypeCatArrayTypeSerialize)
{
	//	Cry::ByteArray baArInt;
	//	ArrayValue valArInt{ 1,2,3,4,5,6,7,8,9,0 };
	//	ArrayValue::Serialize(valArInt, baArInt);
	//	ArrayValue valArIntExp{ 0 };
	//	ArrayValue::Deserialize(valArIntExp, baArInt);
	//
	//	Cry::ByteArray baArUint;
	//	ArrayValue valArUint{ 1U,2U,3U,4U,5U,6U,7U,8U,9U };
	//	ArrayValue::Serialize(valArUint, baArUint);
	//	ArrayValue valArUintExp{ 0 };
	//	ArrayValue::Deserialize(valArUintExp, baArUint);
	//
	//	BOOST_REQUIRE((std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 0}) == valArIntExp.As<int>());
	//	BOOST_REQUIRE((std::vector<unsigned int>{1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U}) == valArUintExp.As<unsigned int>());
}

BOOST_AUTO_TEST_CASE(TypeCatArrayTypeMisc)
{
	//	ArrayValue valArInt{ 1,2,3,4,5,6,7,8,9,0 };
	//	ArrayValue valArInt2{ 11,12,13,14,15,16,17,18,19 };
	//	ArrayValue valCopy{ valArInt };
	//	ArrayValue valMove{ std::move(valCopy) };
	//	valArInt = { 11,12,13,14,15,16,17,18,19 };
	//
	//	BOOST_REQUIRE((std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 0}) == valMove.As<int>());
	//	BOOST_REQUIRE((std::vector<int>{11, 12, 13, 14, 15, 16, 17, 18, 19}) == valArInt.As<int>());
	//
	//	BOOST_REQUIRE(valArInt == valArInt2);
	//	BOOST_REQUIRE(!(valArInt == valMove));
}

//
// RecordType.
//

BOOST_AUTO_TEST_CASE(TypeCatRecordType)
{
	// Test record size.
	/*{
		RecordValue record;
		record.AddField(0, Util::MakeInt(12));

		BOOST_REQUIRE_EQUAL(record.Size(), 1);
	}*/

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

BOOST_AUTO_TEST_CASE(TypeCatRecordTypeSerialize)
{
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

BOOST_AUTO_TEST_CASE(TypeCatRecordTypeMisc)
{
	//BOOST_REQUIRE_EQUAL(0, RecordValue{}.Size());

	/*{
		auto value = RecordValue::AutoValue(Util::MakeInt(81827));

		RecordValue record;
		record.AddField({ "i", value });

		BOOST_REQUIRE_THROW(record.AddField({ "i", value }), RecordValue::FieldExistException);
	}*/
}

//
// VariadicType.
//

BOOST_AUTO_TEST_CASE(TypeCatVariadicType)
{
	//TODO:
}

BOOST_AUTO_TEST_CASE(TypeCatVariadicTypeSerialize)
{
	//TODO:
}

BOOST_AUTO_TEST_CASE(TypeCatVariadicTypeMisc)
{
	//TODO:
}

BOOST_AUTO_TEST_SUITE_END()
