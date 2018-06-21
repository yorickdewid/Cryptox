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
// Test        : ByteStream unittest
// Type        : unit
// Description : -
//

BOOST_AUTO_TEST_SUITE(ByteStream)

BOOST_AUTO_TEST_CASE(BSSimpleOut)
{
	Cry::ByteOutStream bs;

	bs << 186721583;
	bs << 'X';

	BOOST_REQUIRE(!bs.empty());
	BOOST_REQUIRE_EQUAL(bs.size(), 8);
}

BOOST_AUTO_TEST_CASE(BSSimpleIn)
{
	Cry::ByteInStream bs;

	BOOST_REQUIRE(bs.empty());
	BOOST_REQUIRE_EQUAL(bs.size(), 0);
}

BOOST_AUTO_TEST_CASE(BSSimpleIO)
{
	Cry::ByteStream bs;

	{
		bs << 186721583;
		bs << 'X';
		bs << 896127L;
		bs << (short)896127;
		bs << 4223372036854775807LL;
		bs << 18446744073709551614ULL;
	}

	BOOST_REQUIRE(!bs.empty());
	BOOST_REQUIRE_EQUAL(bs.size(), 30);

	{
		int i, j;
		long x;
		short s;
		long long u;
		unsigned long long o;
		bs >> i;
		bs >> j;
		bs >> x;
		bs >> s;
		bs >> u;
		bs >> o;

		BOOST_REQUIRE_EQUAL(186721583, i);
		BOOST_REQUIRE_EQUAL('X', j);
		BOOST_REQUIRE_EQUAL(896127l, x);
		BOOST_REQUIRE_EQUAL((short)896127, s);
		BOOST_REQUIRE_EQUAL(4223372036854775807LL, u);
		BOOST_REQUIRE_EQUAL(18446744073709551614ULL, o);
	}
}

BOOST_AUTO_TEST_CASE(BSSimpleFloat)
{
	{
		Cry::ByteStream bs;

		bs << 129.872f;

		float f;
		bs >> f;

		BOOST_REQUIRE_EQUAL(129.872f, f);
	}

	{
		Cry::ByteStream bs;

		bs << 612873.71536272997;

		double d;
		bs >> d;

		BOOST_REQUIRE_EQUAL(612873.715362729971, d);
	}
}

BOOST_AUTO_TEST_CASE(BSInByteStream)
{
	Cry::ByteStream bs;
	Cry::ByteStream bs2;
	Cry::ByteStream bs3;

	bs << 91;

	bs2 << bs;
	bs2 >> bs3;

	int i;
	bs3 >> i;
	BOOST_REQUIRE_EQUAL(91, i);
}

BOOST_AUTO_TEST_CASE(BSMethods)
{
	Cry::ByteStream bs;

	{
		int k[] = { 121,23,12,34 };
		bs.Write(k, sizeof(k) / sizeof(k[0]));
		bs.Put((short)8672);

		BOOST_REQUIRE_EQUAL(bs.size(), 18);
	}

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

BOOST_AUTO_TEST_SUITE_END()
