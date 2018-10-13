// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/AST.h>

#include <Cry/Algorithm.h>
#include <Cry/Serialize.h>

#include <boost/test/unit_test.hpp>

//
// Key         : Loc
// Test        : Source location unit test
// Type        : unit
// Description : -.
//

BOOST_AUTO_TEST_SUITE(SourceLocation)

BOOST_AUTO_TEST_CASE(SourceLocation)
{
	// Equal operator / not equal operator.
	{
		CryCC::SourceLocation loc1{ 12, 23 };
		CryCC::SourceLocation loc2{ 12, 23 };
		CryCC::SourceLocation loc3{ 12, 24 };

		BOOST_REQUIRE(loc1 == loc2);
		BOOST_REQUIRE(loc1 != loc3);
	}

	// Less operator.
	{
		CryCC::SourceLocation loc1{ 8, 192 };
		CryCC::SourceLocation loc2{ 9, 12 };
		CryCC::SourceLocation loc3{ 9, 88 };
		BOOST_REQUIRE(loc1 < loc2);
		BOOST_REQUIRE(loc2 < loc3);
	}

	// Greater operator.
	{
		CryCC::SourceLocation loc1{ 45, 9 };
		CryCC::SourceLocation loc2{ 34, 56 };
		CryCC::SourceLocation loc3{ 34, 23 };
		BOOST_REQUIRE(loc1 > loc2);
		BOOST_REQUIRE(loc2 > loc3);
	}

	// Less or equal operator.
	{
		CryCC::SourceLocation loc1{ 1162, 19 };
		CryCC::SourceLocation loc2{ 8261, 5 };
		CryCC::SourceLocation loc3{ 1162, 20 };
		BOOST_REQUIRE(loc1 <= loc1);
		BOOST_REQUIRE(loc1 <= loc2);
		BOOST_REQUIRE(loc1 <= loc3);
	}

	// Greater or equal operator.
	{
		CryCC::SourceLocation loc1{ 120, 18 };
		CryCC::SourceLocation loc2{ 91, 72 };
		CryCC::SourceLocation loc3{ 120, 17 };
		BOOST_REQUIRE(loc1 >= loc1);
		BOOST_REQUIRE(loc1 >= loc2);
		BOOST_REQUIRE(loc1 >= loc3);
	}

	// Addition operator.
	{
		CryCC::SourceLocation loc1{ 120, 18 };
		CryCC::SourceLocation loc2{ 91, 72 };

		CryCC::SourceLocation loc3 = loc1 + loc2;
		CryCC::SourceLocation loc4{ 211, 90 };
		BOOST_REQUIRE(loc3 == loc4);
	}

	// Minus operator.
	{
		CryCC::SourceLocation loc1{ 927, 61 };
		CryCC::SourceLocation loc2{ 173, 15 };

		CryCC::SourceLocation loc3 = loc1 - loc2;
		CryCC::SourceLocation loc4{ 754, 46 };
		BOOST_REQUIRE(loc3 == loc4);
	}
}

BOOST_AUTO_TEST_SUITE_END()
