// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <Cry/Cry.h>
#include <Cry/ByteStream.h>

#include <boost/test/unit_test.hpp>

//
// Key         : ByteStream
// Test        : ByteStream unittest
// Type        : unit
// Description : Stream native datatypes and other objects
//               in the byte stream. The byte stream must be
//               able to convert the stream back to the original
//               objects.
//

using namespace Cry::ByteStream;

BOOST_AUTO_TEST_SUITE(ByteStream)

BOOST_AUTO_TEST_CASE(ByteStreamSimpleOut)
{
	ByteOutStream bs;

	bs << 186721583;  // int
	bs << 'X';        // char

	BOOST_REQUIRE(!bs.Empty());
	BOOST_REQUIRE_EQUAL(bs.Size(), 5);
}

BOOST_AUTO_TEST_CASE(ByteStreamSimpleIn)
{
	ByteInStream bs;

	BOOST_REQUIRE(bs.Empty());
	BOOST_REQUIRE_EQUAL(bs.Size(), 0);
}

BOOST_AUTO_TEST_CASE(ByteStreamSimpleIO)
{
	VectorStream bs;

	// Write to stream.
	{
		bs << 186721583;                // int
		bs << 'X';                      // char
		bs << 896127L;                  // long
		bs << (short)896;               // short
		bs << 4223372036854775807LL;    // long long
		bs << 18446744073709551614ULL;  // unsigned long long
		bs << (std::byte)0x71;          // std::byte
	}

	BOOST_REQUIRE(!bs.Empty());

	// Read from stream.
	{
		int i;
		char j;
		long x;
		short s;
		long long u;
		unsigned long long o;
		std::byte b;
		bs >> i;   // int
		bs >> j;   // char
		bs >> x;   // long
		bs >> s;   // short
		bs >> u;   // long long
		bs >> o;   // unsigned long long
		bs >> b;   // std::byte

		BOOST_REQUIRE_EQUAL(186721583, i);
		BOOST_REQUIRE_EQUAL('X', j);
		BOOST_REQUIRE_EQUAL(896127l, x);
		BOOST_REQUIRE_EQUAL((short)896, s);
		BOOST_REQUIRE_EQUAL(4223372036854775807LL, u);
		BOOST_REQUIRE_EQUAL(18446744073709551614ULL, o);
		BOOST_REQUIRE((std::byte)0x71 == b);
	}
}

BOOST_AUTO_TEST_CASE(ByteStreamSimpleFloat)
{
	// Test float.
	{
		VectorStream bs;

		bs << 129.872f;

		float f;
		bs >> f;

		BOOST_REQUIRE_EQUAL(129.872f, f);
	}

	// Test double.
	{
		VectorStream bs;

		bs << 612873.71536272997;

		double d;
		bs >> d;

		BOOST_REQUIRE_EQUAL(612873.715362729971, d);
	}

	// Test long double.
	{
		VectorStream bs;

		bs << (long double)874.274624352;

		long double ld;
		bs >> ld;

		BOOST_REQUIRE_EQUAL(874.27462435200005, ld);
	}
}

BOOST_AUTO_TEST_CASE(ByteStreamInByteStream)
{
	VectorStream bs;
	VectorStream bs2;
	VectorStream bs3;

	bs << 91;

	bs2 << bs;
	bs2 >> bs3;

	int i;
	bs3 >> i;
	BOOST_REQUIRE_EQUAL(91, i);
}

BOOST_AUTO_TEST_CASE(ByteStreamVector)
{
	using namespace std::string_literals;

	// Test vector.
	{
		VectorStream bs;

		auto list = { 1, 2, 3, 4 };   // initializer list of integers
		bs << list;

		std::vector<int> list2;
		bs >> list2;
		BOOST_REQUIRE_EQUAL_COLLECTIONS(list.begin(), list.end(), list2.begin(), list2.end());
	}

	// Test std::string.
	{
		VectorStream bs;

		bs << "kaas"s;   // std::string

		std::string str;
		bs >> str;

		BOOST_REQUIRE_EQUAL("kaas"s, str);
	}
}

BOOST_AUTO_TEST_CASE(ByteStreamMethods)
{
	VectorStream bs;

	// Write direct bytes.
	{
		int k[] = { 121,23,12,34 };
		bs.Write(k, sizeof(k) / sizeof(k[0]));
		bs.Put((short)8672);

		BOOST_REQUIRE_EQUAL(bs.Size(), 18);
	}

	// Read direct bytes.
	{
		int x[] = { 0,0,0,0 };
		bs.Read(&x, sizeof(x) / sizeof(x[0]));
		short s;
		bs.Get(s);

		BOOST_REQUIRE_EQUAL(121, x[0]);
		BOOST_REQUIRE_EQUAL(23, x[1]);
		BOOST_REQUIRE_EQUAL(12, x[2]);
		BOOST_REQUIRE_EQUAL(34, x[3]);
		BOOST_REQUIRE_EQUAL((short)8672, s);
	}
}

class MyClass
{
public:
	MyClass(int i)
		: m_i{ i }
	{
	}

	explicit MyClass(VectorStream& bs)
	{
		bs >> m_i;
	}

	friend VectorStream& operator<<(VectorStream& s, const MyClass& other)
	{
		s << other.m_i << other.m_c;
		return s;
	}

	bool operator==(const MyClass& other)
	{
		return m_i == other.m_i
			&& m_c == other.m_c;
	}

private:
	int m_i;
	char m_c{ 'Y' };
};

BOOST_AUTO_TEST_CASE(ByteStreamCustomObject)
{
	VectorStream bs;
	MyClass myclass{ 17 };

	bs << myclass;

	BOOST_REQUIRE(myclass == MyClass{ bs });
}

BOOST_AUTO_TEST_CASE(ByteStreamStreamBarrier)
{
	VectorStream bs;

	bs << 2738ULL;
	bs << VectorStream::StreamBarrier;
	bs << 'U';

	unsigned long long t;
	bs >> t;

	BOOST_CHECK_EQUAL(2738ULL, t);
	BOOST_REQUIRE(bs.HasStreamBarrier());
	BOOST_REQUIRE(!bs.HasStreamBarrier());

	char u;
	bs >> u;
	BOOST_CHECK_EQUAL('U', u);
}

BOOST_AUTO_TEST_SUITE_END()
