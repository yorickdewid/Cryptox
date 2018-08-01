// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "../test/10-manifest.h"

#include <boost/test/unit_test.hpp>

//
// Test        : Auto generated event log unit test
// Type        : unit
// Description : -
//

BOOST_AUTO_TEST_SUITE(EventLogException)

BOOST_AUTO_TEST_CASE(ClUnitSimpleExcept)
{
	auto except = EventLog::MakeException(10);

	BOOST_REQUIRE_EQUAL(except.Event().errorCode, 10);
}

BOOST_AUTO_TEST_SUITE_END()
