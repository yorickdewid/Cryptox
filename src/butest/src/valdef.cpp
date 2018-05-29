// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "../src/Valuedef.h"

#include <boost/test/unit_test.hpp>

//
// Test        : Value definition unitttest
// Type        : unit
// Description : -
//

using namespace CoilCl;

BOOST_AUTO_TEST_SUITE(ValueDefinition)

BOOST_AUTO_TEST_CASE(ValDefBasicLegacy)
{
	//NOTE: Float and string fail

	auto valInt = Util::MakeInt(12);
	auto valDouble = Util::MakeDouble(7123.7263812);
	auto valChar = Util::MakeChar('x');
	auto valBool = Util::MakeBool(true);
	auto valVoid = Util::MakeVoid();

	BOOST_CHECK(!Util::IsValueArray(valInt));
	BOOST_CHECK(!Util::IsValueArray(valDouble));
	BOOST_CHECK(!Util::IsValueArray(valChar));
	BOOST_CHECK(!Util::IsValueArray(valBool));

	BOOST_REQUIRE_EQUAL(12, valInt->As<int>());
	BOOST_REQUIRE_EQUAL(7123.7263812, valDouble->As<double>());
	BOOST_REQUIRE_EQUAL('x', valChar->As<char>());
	BOOST_REQUIRE_EQUAL(true, valBool->As<bool>());
	
	BOOST_CHECK(Util::IsValueVoid(valVoid));
}

BOOST_AUTO_TEST_SUITE_END()
