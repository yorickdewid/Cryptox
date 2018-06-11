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
//               usage of the ByteArray structure. The ByteArray
//               should be used throughout the project whenever
//               data serialization is required.
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

	buffer2.StartOffset(static_cast<int>(buffer2.Offset() + testStr.size()));
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
	BOOST_REQUIRE_NE(var_3, buffer.Deserialize<Cry::Byte>(4));
	BOOST_REQUIRE_EQUAL(var_3, buffer.Deserialize<Cry::Byte>(5));
}

BOOST_AUTO_TEST_CASE(BAInBA)
{
	static const Cry::Word var_1 = 510238712;
	static const Cry::Word var_2 = 107389278;
	static const Cry::Word var_3 = 718281581;

	Cry::ByteArray buffer_inner;
	buffer_inner.SetMagic(9);
	buffer_inner.SetPlatformCompat();
	buffer_inner.Serialize(var_3);

	Cry::ByteArray buffer;
	buffer.SetMagic(199);
	buffer.SetPlatformCompat();
	buffer.Serialize(var_1);
	buffer.Serialize(var_2);
	buffer.SerializeAs<Cry::Short>(buffer_inner.size());
	buffer.insert(buffer.cend(), buffer_inner.begin(), buffer_inner.end());

	Cry::ByteArray buffer2;
	buffer2.resize(buffer.size());
	std::copy(buffer.cbegin(), buffer.cend(), buffer2.begin());

	BOOST_REQUIRE(buffer2.ValidateMagic(199));
	BOOST_REQUIRE(buffer2.IsPlatformCompat());
	BOOST_REQUIRE_EQUAL(var_1, buffer2.Deserialize<Cry::Word>(Cry::ByteArray::AUTO));
	BOOST_REQUIRE_EQUAL(var_2, buffer2.Deserialize<Cry::Word>(Cry::ByteArray::AUTO));

	Cry::ByteArray buffer_inner2;
	size_t buffer_inner2_size = buffer2.Deserialize<Cry::Short>(Cry::ByteArray::AUTO);
	std::copy(buffer2.cbegin() + buffer2.Offset(), buffer2.cbegin() + buffer2.Offset() + buffer_inner2_size, std::back_inserter(buffer_inner2));
	BOOST_REQUIRE(buffer_inner2.ValidateMagic(9));
	BOOST_REQUIRE(buffer_inner2.IsPlatformCompat());
	BOOST_REQUIRE_EQUAL(var_3, buffer_inner2.Deserialize<Cry::Word>(Cry::ByteArray::AUTO));
}

BOOST_AUTO_TEST_CASE(BAContainer)
{
	static const Cry::Word var_1 = 712361582;
	static const Cry::Word var_2 = 812785362;
	static const Cry::Word var_3 = 761836176;

	std::vector<int> v = { 12, 82, 781, 22 };

	Cry::ByteArray buffer;
	buffer.SetMagic(241);
	buffer.SetPlatformCompat();
	buffer.Serialize(var_1);
	buffer.Serialize(var_2);
	buffer.SerializeAs<Cry::Short>(v.size());
	buffer.insert(buffer.cend(), v.cbegin(), v.cend());
	buffer.Serialize(var_3);
	
	buffer.StartOffset(0);
	BOOST_REQUIRE(buffer.ValidateMagic(241));
	BOOST_REQUIRE(buffer.IsPlatformCompat());
	BOOST_REQUIRE_EQUAL(var_1, buffer.Deserialize<Cry::Word>());
	BOOST_REQUIRE_EQUAL(var_2, buffer.Deserialize<Cry::Word>());

	std::vector<int> v2;
	size_t v2_size = buffer.Deserialize<Cry::Short>();
	std::copy(buffer.cbegin() + buffer.Offset(), buffer.cbegin() + buffer.Offset() + v2_size, std::back_inserter(v2));
	buffer.SetOffset(static_cast<int>(v2_size));

	BOOST_REQUIRE_EQUAL(v2.size(), v.size());
	BOOST_REQUIRE_EQUAL(var_3, buffer.Deserialize<Cry::Word>());
}

BOOST_AUTO_TEST_CASE(BAOperators)
{
	{
		Cry::ByteArray buffer;
		buffer.SetMagic(0x2f);
		buffer.SetPlatformCompat();
		buffer.SerializeAs<Cry::Word>(897128934);

		Cry::ByteArray buffer2;
		buffer2.SetMagic(0x2f);
		buffer2.SetPlatformCompat();
		buffer2.SerializeAs<Cry::Word>(897128934);

		BOOST_REQUIRE(buffer == buffer2);
	}

	{
		Cry::ByteArray buffer;
		buffer.SetMagic(0x2f);
		buffer.SerializeAs<Cry::Word>(872);
		buffer.SerializeAs<Cry::Byte>('X');
		buffer.StartOffset(1);

		BOOST_REQUIRE_EQUAL(872, buffer.Deserialize<Cry::Word>());
		BOOST_REQUIRE_EQUAL('X', buffer.Deserialize<Cry::Byte>());
	}
}

BOOST_AUTO_TEST_SUITE_END()
