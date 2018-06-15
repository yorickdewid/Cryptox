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
		bs << 896127l;
	}

	BOOST_REQUIRE(!bs.empty());
	BOOST_REQUIRE_EQUAL(bs.size(), 12);

	{
		int i, j;
		long x;
		bs >> i;
		bs >> j;
		bs >> x;

		BOOST_REQUIRE_EQUAL(186721583, i);
		BOOST_REQUIRE_EQUAL('X', j);
		BOOST_REQUIRE_EQUAL(896127l, x);
	}
}

BOOST_AUTO_TEST_SUITE_END()
