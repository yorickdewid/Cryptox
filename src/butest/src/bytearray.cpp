// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <Cry/Cry.h>
#include <Cry/Serialize.h>

#include <boost/test/unit_test.hpp>

//
// Test        : ByteArray unittest
// Type        : unit
// Description : Test the possible combinations and common
//               usage of the ByteArray structure.
//

BOOST_AUTO_TEST_SUITE(ByteArray)

BOOST_AUTO_TEST_CASE(BASimpleEnDecode)
{
	static const char char_1 = 'A';
	static const int char_2 = 16276;

	Cry::ByteArray buffer;
	buffer.SerializeAs<Cry::Byte>(char_1);
	buffer.SerializeAs<Cry::Short>(char_2);

	BOOST_REQUIRE_EQUAL(buffer.size(), 3);
	BOOST_REQUIRE_EQUAL(char_1, buffer.Deserialize<Cry::Byte>(Cry::ByteArray::AUTO));
	BOOST_REQUIRE_EQUAL(char_2, buffer.Deserialize<Cry::Short>(Cry::ByteArray::AUTO));
}

BOOST_AUTO_TEST_CASE(BAMagicCheck)
{
	static const Cry::Byte magic = static_cast<Cry::Byte>('\xe2');

	static const char char_1 = 'A';
	static const int char_2 = 16276;

	Cry::ByteArray buffer;
	buffer.SetMagic(magic);
	buffer.SerializeAs<Cry::Byte>(char_1);
	buffer.SerializeAs<Cry::Short>(char_2);
	buffer.SerializeAs<Cry::Short>(char_2);
	buffer.SerializeAs<Cry::Short>(char_2);

	BOOST_REQUIRE_EQUAL(buffer.size(), 8);

	Cry::ByteArray buffer2;
	std::copy(buffer.cbegin(), buffer.cend(), std::back_inserter(buffer2));
	BOOST_REQUIRE_EQUAL(buffer2.size(), 8);

	BOOST_REQUIRE(buffer2.ValidateMagic(magic));
	BOOST_REQUIRE_EQUAL(char_1, buffer2.Deserialize<Cry::Byte>(Cry::ByteArray::AUTO));
	BOOST_REQUIRE_EQUAL(char_2, buffer2.Deserialize<Cry::Short>(Cry::ByteArray::AUTO));
	BOOST_REQUIRE_EQUAL(char_2, buffer2.Deserialize<Cry::Short>(Cry::ByteArray::AUTO));
	BOOST_REQUIRE_EQUAL(char_2, buffer2.Deserialize<Cry::Short>(Cry::ByteArray::AUTO));
}

BOOST_AUTO_TEST_CASE(BAOSCompatibleWithString)
{
	static const Cry::Word var_1 = 276478381;
	static const Cry::Short var_2 = 1292;
	static const Cry::Byte var_3 = 99;

	std::string testStr{ "teststr" };

	Cry::ByteArray buffer;
	buffer.SetMagic(127);
	buffer.SetPlatformCompat();
	buffer.Serialize(var_1);
	buffer.insert(buffer.cend(), testStr.begin(), testStr.end());
	buffer.Serialize(var_2);
	buffer.Serialize(var_3);

	BOOST_REQUIRE_EQUAL(buffer.size(), 6 + testStr.size() + 3);

	Cry::ByteArray buffer2;
	buffer2.resize(6 + testStr.size() + 3);
	std::copy(buffer.cbegin(), buffer.cend(), buffer2.begin());

	BOOST_REQUIRE(buffer2.ValidateMagic(127));
	BOOST_REQUIRE(buffer2.IsPlatformCompat());
	BOOST_REQUIRE_EQUAL(var_1, buffer2.Deserialize<Cry::Word>(Cry::ByteArray::AUTO));

	std::string testStr2;
	std::copy(buffer2.cbegin() + buffer2.Offset(), buffer2.cbegin() + buffer2.Offset() + testStr.size(), std::back_inserter(testStr2));
	BOOST_REQUIRE_EQUAL(testStr, testStr2);

	buffer2.StartOffset(buffer2.Offset() + testStr.size());
	BOOST_REQUIRE_EQUAL(var_2, buffer2.Deserialize<Cry::Short>(Cry::ByteArray::AUTO));
	BOOST_REQUIRE_EQUAL(var_3, buffer2.Deserialize<Cry::Byte>(Cry::ByteArray::AUTO));
}

BOOST_AUTO_TEST_CASE(BADecodeError)
{
	static const Cry::Word var_1 = 892613789;
	static const Cry::Byte var_3 = 61;

	Cry::ByteArray buffer;
	buffer.Serialize(var_1);
	buffer.SetMagic(254);
	buffer.Serialize(var_3);

	BOOST_REQUIRE(!buffer.ValidateMagic(254));
	buffer.StartOffset(sizeof(var_1));
	BOOST_REQUIRE(buffer.ValidateMagic(254));
	buffer.StartOffset(0);
	BOOST_REQUIRE(buffer.ValidateMagic(254, 4));
	BOOST_REQUIRE_NE(var_3, buffer.Deserialize<Cry::Byte>(Cry::ByteArray::AUTO));
}

BOOST_AUTO_TEST_SUITE_END()
