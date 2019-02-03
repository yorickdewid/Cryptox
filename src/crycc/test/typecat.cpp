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
	NilType tyCopy{ tyNil };
	NilType tyMove{ std::move(tyCopy) };

	BOOST_REQUIRE(tyNil == tyMove);
}

//
// VariantType.
//

BOOST_AUTO_TEST_CASE(TypeCatVariantType)
{
	VariantType tyVar3{ Util::MakeNilType(), Util::MakePointerType(Util::MakeNilType()), Util::MakeBuiltinType(BuiltinType::Specifier::VOID_T) };
	VariantType tyVar2{ Util::MakeBuiltinType(BuiltinType::Specifier::LONG_DOUBLE_T), Util::MakeNilType() };
	VariantType tyVar1{ Util::MakeBuiltinType(BuiltinType::Specifier::INT_T) };

	BOOST_REQUIRE_EQUAL(3, tyVar3.Order());
	BOOST_REQUIRE_EQUAL(2, tyVar2.Order());
	BOOST_REQUIRE_EQUAL(1, tyVar1.Order());
}

BOOST_AUTO_TEST_CASE(TypeCatVariantTypeSerialize)
{
	using namespace Cry::ByteStream;

	{
		VectorStream veType;
		VariantType tyVar2{ Util::MakeBuiltinType(BuiltinType::Specifier::INT_T), Util::MakeBuiltinType(BuiltinType::Specifier::SIGNED_CHAR_T) };
		VariantType::Serialize(tyVar2, veType);
		VariantType tyVar2Exp{};
		VariantType::Deserialize(tyVar2Exp, veType);

		// TODO: 
		BOOST_REQUIRE(tyVar2Exp == tyVar2);
	}

	{
		VectorStream veType;
		VariantType tyVar1{ Util::MakeBuiltinType(BuiltinType::Specifier::UNSIGNED_INT_T) };
		VariantType::Serialize(tyVar1, veType);
		VariantType tyVar1Exp{ };
		VariantType::Deserialize(tyVar1Exp, veType);

		// TODO: 
		BOOST_REQUIRE(tyVar1Exp == tyVar1);
	}
}
BOOST_AUTO_TEST_CASE(TypeCatVariantTypeMisc)
{
	VariantType tyVar2{ Util::MakeBuiltinType(BuiltinType::Specifier::DOUBLE_T), Util::MakeBuiltinType(BuiltinType::Specifier::SHORT_T) };
	VariantType tyCopy{ tyVar2 };
	VariantType tyMove{ std::move(tyCopy) };

	// TODO: 
	BOOST_REQUIRE(tyVar2 == tyMove);
}

//
// TypedefType.
//

BOOST_AUTO_TEST_CASE(TypeCatTypedefType)
{
	TypedefType tyDef{ "newtype", Util::MakeNilType() };

	BOOST_REQUIRE_EQUAL("newtype", tyDef.Name());
	BOOST_REQUIRE_EQUAL(0, tyDef.UnboxedSize());
	BOOST_REQUIRE(TypeVariation::NIL == tyDef.MarkType()->TypeId());
}

BOOST_AUTO_TEST_CASE(TypeCatTypedefTypeSerialize)
{
	using namespace Cry::ByteStream;

	VectorStream veType;
	TypedefType tyDef{ "alias", Util::MakeBuiltinType(BuiltinType::Specifier::INT_T) };
	TypedefType::Serialize(tyDef, veType);
	TypedefType tyDefExp{ {}, Util::MakeNilType() };
	TypedefType::Deserialize(tyDefExp, veType);

	BOOST_REQUIRE(tyDefExp == tyDef);
}

BOOST_AUTO_TEST_CASE(TypeCatTypedefTypeMisc)
{
	TypedefType tyDef{ "pointer", Util::MakePointerType(Util::MakeNilType()) };
	TypedefType tyCopy{ tyDef };
	TypedefType tyMove{ std::move(tyCopy) };

	BOOST_REQUIRE(tyDef == tyMove);
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
		BuiltinType tyIntExp{ BuiltinType::Specifier::VOID_T };
		BuiltinType::Deserialize(tyIntExp, veType);

		// TODO: 
		BOOST_REQUIRE(tyIntExp == tyInt);
	}

	{
		VectorStream veType;
		BuiltinType tyUint{ BuiltinType::Specifier::UNSIGNED_INT_T };
		BuiltinType::Serialize(tyUint, veType);
		BuiltinType tyUintExp{ BuiltinType::Specifier::VOID_T };
		BuiltinType::Deserialize(tyUintExp, veType);

		// TODO: 
		BOOST_REQUIRE(tyUintExp == tyUint);
	}
}

BOOST_AUTO_TEST_CASE(TypeCatBuiltinTypeMisc)
{
	BuiltinType tyUInt{ BuiltinType::Specifier::UNSIGNED_INT_T };
	BuiltinType tyCopy{ tyUInt };
	BuiltinType tyMove{ std::move(tyCopy) };

	// TODO: 
	BOOST_REQUIRE(tyUInt == tyMove);
}

//
// ArrayType.
//

BOOST_AUTO_TEST_CASE(TypeCatArrayType)
{
	ArrayType tyArNil{ 3, Util::MakeNilType() };
	ArrayType tyArVoid{ 2, Util::MakeBuiltinType(BuiltinType::Specifier::VOID_T) };
	ArrayType tyArBool{ 9, Util::MakeBuiltinType(BuiltinType::Specifier::BOOL_T) };
	ArrayType tyArInt{ 24, Util::MakeBuiltinType(BuiltinType::Specifier::INT_T) };

	BOOST_REQUIRE_EQUAL(3, tyArNil.Order());
	BOOST_REQUIRE_EQUAL(2, tyArVoid.Order());
	BOOST_REQUIRE_EQUAL(9, tyArBool.Order());
	BOOST_REQUIRE_EQUAL(24, tyArInt.Order());
}

BOOST_AUTO_TEST_CASE(TypeCatArrayTypeSerialize)
{
	using namespace Cry::ByteStream;

	{
		VectorStream veType;
		ArrayType tyArInt{ 7, Util::MakeBuiltinType(BuiltinType::Specifier::INT_T) };
		ArrayType::Serialize(tyArInt, veType);
		ArrayType tyArIntExp{ 0, Util::MakeNilType() };
		ArrayType::Deserialize(tyArIntExp, veType);

		// TODO: 
		BOOST_REQUIRE(tyArIntExp == tyArInt);
	}

	{
		VectorStream veType;
		ArrayType tyUint{ 4, Util::MakeBuiltinType(BuiltinType::Specifier::UNSIGNED_INT_T) };
		ArrayType::Serialize(tyUint, veType);
		ArrayType tyUintExp{ 0, Util::MakeBuiltinType(BuiltinType::Specifier::VOID_T) };
		ArrayType::Deserialize(tyUintExp, veType);

		// TODO: 
		BOOST_REQUIRE(tyUintExp == tyUint);
	}
}

BOOST_AUTO_TEST_CASE(TypeCatArrayTypeMisc)
{
	ArrayType tyArUInt{ 5, Util::MakeBuiltinType(BuiltinType::Specifier::DOUBLE_T) };
	ArrayType tyCopy{ tyArUInt };
	ArrayType tyMove{ std::move(tyCopy) };

	// TODO: 
	BOOST_REQUIRE(tyArUInt == tyMove);
}

//
// RecordType.
//

BOOST_AUTO_TEST_CASE(TypeCatRecordType)
{
	{
		RecordType tyRec{ "somestruct" };
		tyRec.AddField(RecordType::FieldType{ "someint", Util::MakeBuiltinType(BuiltinType::Specifier::INT_T) });
		BOOST_REQUIRE(!tyRec.IsAligned());
		BOOST_REQUIRE(!tyRec.IsAnonymous());
		BOOST_REQUIRE_EQUAL(tyRec.Name(), "somestruct");
		BOOST_REQUIRE_EQUAL(tyRec.FieldSize(), 1);
		BOOST_REQUIRE_EQUAL(tyRec.UnboxedSize(), Util::MakeBuiltinType(BuiltinType::Specifier::INT_T)->UnboxedSize());
	}

	{
		RecordType tyRec;
		BOOST_REQUIRE(tyRec.IsAnonymous());
		BOOST_REQUIRE_EQUAL(tyRec.FieldSize(), 0);
		BOOST_REQUIRE_EQUAL(tyRec.UnboxedSize(), 0);
	}

	{
		RecordType tyRec{ RecordType::Specifier::UNION };
		tyRec.AddField("null", Util::MakeNilType());
		tyRec.AddField("null2", Util::MakeNilType());
		tyRec.AddField("null3", Util::MakeNilType());
		BOOST_REQUIRE(tyRec.TypeSpecifier() == RecordType::Specifier::UNION);
		BOOST_REQUIRE_EQUAL(tyRec.FieldSize(), 3);
		BOOST_REQUIRE_EQUAL(tyRec.UnboxedSize(), 0);
	}
}

BOOST_AUTO_TEST_CASE(TypeCatRecordTypeSerialize)
{
	using namespace Cry::ByteStream;

	{
		VectorStream veType;
		RecordType tyRec{ "struct" };
		tyRec.AddField("int", Util::MakeBuiltinType(BuiltinType::Specifier::INT_T));
		tyRec.AddField("null", Util::MakeNilType());
		RecordType::Serialize(tyRec, veType);
		RecordType tyRecExp;
		RecordType::Deserialize(tyRecExp, veType);

		// TODO: 
		BOOST_REQUIRE(tyRecExp == tyRec);
	}

	{
		VectorStream veType;
		RecordType tyRec{ "union", RecordType::Specifier::UNION };
		tyRec.AddField("int", Util::MakeBuiltinType(BuiltinType::Specifier::UNSIGNED_LONG_DOUBLE_T));
		tyRec.AddField("null", Util::MakeNilType());
		tyRec.AddField("null2", Util::MakeNilType());
		RecordType::Serialize(tyRec, veType);
		RecordType tyRecExp;
		RecordType::Deserialize(tyRecExp, veType);

		// TODO: 
		BOOST_REQUIRE(tyRecExp == tyRec);
	}
}

BOOST_AUTO_TEST_CASE(TypeCatRecordTypeMisc)
{
	RecordType tyRec;
	RecordType tyCopy{ tyRec };
	RecordType tyMove{ std::move(tyCopy) };

	BOOST_REQUIRE(tyRec == tyMove);
}

//
// VariadicType.
//

BOOST_AUTO_TEST_CASE(TypeCatVariadicType)
{
	VariadicType tyVa;

	BOOST_REQUIRE_EQUAL("...", tyVa.ToString());
	BOOST_REQUIRE_EQUAL(0, tyVa.UnboxedSize());
	BOOST_REQUIRE(TypeVariation::VARIADIC == tyVa.TypeId());
}

BOOST_AUTO_TEST_CASE(TypeCatVariadicTypeSerialize)
{
	using namespace Cry::ByteStream;

	VectorStream veType;
	VariadicType tyVa;
	tyVa.SetQualifier(NilType::TypeQualifier::VOLATILE_T);
	VariadicType::Serialize(tyVa, veType);
	VariadicType tyVaExp;
	VariadicType::Deserialize(tyVaExp, veType);

	BOOST_REQUIRE(tyVaExp == tyVa);
}

BOOST_AUTO_TEST_CASE(TypeCatVariadicTypeMisc)
{
	VariadicType tyVar;
	VariadicType tyCopy{ tyVar };
	VariadicType tyMove{ std::move(tyCopy) };

	BOOST_REQUIRE(tyVar == tyMove);
}

BOOST_AUTO_TEST_SUITE_END()
